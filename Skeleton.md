# 1. Skeleton

## 1.1 Load File

[SKEL File Format](https://dartsim.github.io/skel_file_format.html)

The `.skel` file is a hierarchy of joints. Each joint lists some relevant data about its configuration. The `.skel` data file has 8 keyword tokens which specify properties of a particular joint and are followed by data (usually **floats**). Not all joints will specify all properties, so **reasonable default values should be used**.

```c++
balljoint name{						(parent joint)
    offset	    	x y z	    	(joint offset vector)
    boxmin	    	x y z	    	(min corner of box to draw)
    boxmax	    	x y z	    	(max corner of box to draw)
    rotxlimit	    min max	    	(x rotation DOF limits)
    rotylimit	    min max	    	(y rotation DOF limits)
    rotzlimit	    min max	    	(z rotation DOF limits)
    pose	    	x y z	    	(values to pose DOFs)
	balljoint	    name { }	    (child joint)
}
        
// e.g.
balljoint root {
  offset     0.000000     0.000000     0.000000
  pose       0.000000     0.000000     0.000000
  boxmin -0.2 -0.3 -0.3
  boxmax 0.2 0.2 0.3
  balljoint head {
    offset     0.000000     0.000000    -0.400000
    pose       0.000000     0.000000     0.000000
    boxmin -0.2 -0.4 -0.4
    boxmax 0.2 0.1 0.0
  }
  balljoint abdomen {
    offset     0.000000    -0.200000     0.400000
    pose       0.000000     0.000000     0.000000
	boxmin -0.1 -0.1 -0.1
	boxmax 0.1 0.1 0.1
    balljoint tail_01 {
      offset     0.000000     0.400000     0.250000
      pose       0.000000     0.000000     0.000000
	  boxmin -0.3 -0.5 -0.1
	  boxmax 0.3 0.1 0.2
      balljoint tail_02 {

```

- `offset`: constant positional offset to add to the local joint transformation. It represents the <u>location of a joint's pivot point</u> described <u>relative to the parent joint's space</u>. This will almost always be specified for a joint, but if not, it should default to `(0,0,0)`.

- The `boxmin`and `boxmax` parameters describe <u>the min and max corners of a box representing the bone to render</u> for the particular joint. If these are not specified for a joint, they should still have <u>a reasonable default</u>, say `(-0.1,-0.1,-0.1)` and `(0.1,0.1,0.1)`.

- The `rotlimit` tokens describe <u>the rotational joint DOF limits</u>. Note that all angles are in **radians**. These should <u>default to an essentially unlimited state</u> if they are not specified in the file (i.e., -100000 to 100000 would be fine).

- The `pose` token specifies <u>values to pose the DOFs at</u>. Normally, data like this would not be needed in a skeleton file, as the skeleton is usually posed by a higher level animation system. <u>By default, these should be 0</u>. <u>If the pose specifies values outside of the range of the DOF limits, then it should get properly clamped before displaying</u>. Again, remember that these values are in **radians**.

- The `balljoint` token specifies a child balljoint which will have its own data and possibly its own children. There should not be any limit to the number of children allowed. Every balljoint has a name in the file.

> Hint:
>
> - `View Projection Matrix` transforms from world space into the view space of the camera for rendering.
> - Can use the `Cube` class as a box to render a bone. It can be constructed with the `boxmin` & `boxmax` values of a particular bone. To draw it in the correct world-space location based on the joint's world matrix `W`, we multiply it with the camera `view-projection matrix` ` V `to get `V*W`. This should then <u>be passed to the Cube::draw() function.</u>

## 1.2 Classes

### 1.2.1 DOF

- Data 
  - Value 
  - Limits: min & max 
- Methods 
  - SetValue() – (could clamp value to joint limits) 
  - GetValue() 
  - SetMinMax()

### 1.2.2 Cube

- Data
  - positions
  - normals
  - indices
  - modelMtx
  - color
- Methods 
  - buildCube()
  - drawCube()

### 1.2.3 Joint

- Core Joint data 

  - DOFs (N floats) 
  - Local matrix: 𝐋 
  - World matrix: 𝐖 

- Additional data 

  - Joint offset vector
  - DOF limits 
    - Type-specific data (rotation/translation axis, other constants…) 
  - Tree data (array of pointers to children, etc.)

- Functions 

  - Load() 
  - Update(): recursively generate local matrix & concatenate
  - AddChild() 
  - Draw()


> Note: one could also make a **Joint base class** and derive various specific joint types (hinge, ball, prismatic…). In this case, one could have a virtual function called `MakeLocalMatrix()` that the base class `Update()` calls

```c++
bool Joint::Load(Tokenizer &t) {
	token.FindToken("{"));
	while(1) {
		char temp[256];
		token.GetToken(temp);
		if(strcmp(temp,"offset")==0) {
			Offset.x=token.GetFloat();
			Offset.y=token.GetFloat();
			Offset.z=token.GetFloat();
		}
		else if // Check for other tokens
		else if(strcmp(temp,"balljoint")==0) {
			Joint *jnt=new Joint;
			jnt->Load(token);
			AddChild(*jnt);
		}
		else if(strcmp(temp,"}")==0) return true;
		else token.SkipLine(); // Unrecognized token
	}
}
```

### 1.2.4 Skeleton

- Data 

  - Joint tree (possibly just a pointer to the root joint) 

    > Tree Data Structure: 
    >
    > The skeleton requires a basic N-tree data structure. A simple way to do this is to just have the base joint class store an array (vector) of child joints. To do the depth-first traversal in the `Update()`, each joint would just loop through it’s child joints and call their `Update()`.

- Functions 

  - Load(): read file, load joint
  - Update(): Update each joint (traverse tree & compute all joint matrices) 
  - Draw(): draw each joint (traverse tree)

```c++
bool Skeleton::Load(const char *file) {
	Tokenizer token;
	token.Open(file,"skel"));
	token.FindToken("balljoint"));
	// Parse tree
	Root=new Joint;
	Root->Load(token);
	// Finish
	token.Close();
	return true;
}
```

## 1.3 Display

```c++
void Joint::Update(Matrix& parent) {
	… // Compute local matrix L
	… // Compute world matrix W
	… // Recursively call Update() on children
}
void Joint::Draw() {
	… // Draw oriented box with OpenGL
	… // Recursively call Draw() on children
}
```

## 1.4 GUI

> Add a simple GUI to the program (such as ImGui, NanoGUI, or AntTweakBar) that allows the user to interactively adjust any of the DOFs in the skeleton. 

ImGui:

- list all DOFs by name (i.e. "knee_r X") and have a slider for each one
- allows the user to adjust the value within the DOF limits
- display the active DOF name and value on the screen.
- have a next/last joint button that selects the current joint and just displays DOFs for that joint
- a fancy version could allow the user to select a joint in 3D with the mouse and edit the DOFs of that joint.

<u>Whichever way is used, the name of the joint and the values of the DOFs must be displayed somewhere.</u>

lists all of the DOFs by name  and allows the user to adjust the value within the DOF limits.

## 1.5 Features

1. Slider bar changes:
   1. Camera distance, azimuth, inclination;
   2. DOFs of every joint.
2. Press R to reset all DOF.