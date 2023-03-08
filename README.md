# 1. Skeleton

## 1.1 Load SKEL File

[SKEL File Format](https://dartsim.github.io/skel_file_format.html)

The `.skel file` is a hierarchy of joints. Each joint lists some relevant data about its configuration. The `.skel` data file has 8 keyword tokens which specify properties of a particular joint and are followed by data (usually **floats**). Not all joints will specify all properties, so **reasonable default values should be used**.

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

## 1.3 Classes

### 1.3.1 DOF

- Data 
  - Value 
  - Limits: min & max 
- Functions 
  - SetValue() – (could clamp value to joint limits) 
  - GetValue() 
  - SetMinMax()

### 1.3.2 Joint

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



### 1.3.3 Skeleton

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

## 1.4 Display

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



## 1.5 GUI 

> Add a simple GUI to the program (such as ImGui, NanoGUI, or AntTweakBar) that allows the user to interactively adjust any of the DOFs in the skeleton. 

### 1.5.1 ImGui

- list all DOFs by name (i.e. "knee_r X") and have a slider for each one
- allows the user to adjust the value within the DOF limits
- display the active DOF name and value on the screen.
- have a next/last joint button that selects the current joint and just displays DOFs for that joint
- a fancy version could allow the user to select a joint in 3D with the mouse and edit the DOFs of that joint.

<u>Whichever way is used, the name of the joint and the values of the DOFs must be displayed somewhere.</u>

lists all of the DOFs by name  and allows the user to adjust the value within the DOF limits.  **[1 point]**

## 1.6 FUNCTIONS

1. Slider bar changes:
   1. Camera distance, azimuth, inclination;
   2. DOFs of every joint.
2. Press R to reset all DOF.

# 2. Skinning

## 2.1 Load SKIN file

The program can load any .skin and .skel file given to it.

It also works if only one of the files is given. For example, if only the .skel file is given, it should operate as in project 1. If only the skin file is given, it should display the skin mesh in its undeformed state (i.e., in its original binding space).

```c++
if (isSelectSkel) {
    isDrawSkel = true;
    possible to select attach skin if .skin file exists;
    if (isSelectAttachedSkin){
        isDrawSkel = false;
        isDrawAttachedSkin = true;
    }
}
else {
    if (isSelectOriginalSkin){
        
    }
}
```



### 2.1.1 The Skin File

The .skin file contains arrays of vertex data, an array of triangle data, and an array of binding matrices.

```json
positions [numverts] {
    [x] [y] [z]
    ...
}

normals [numverts] {
    [x] [y] [z]
    ...
}

skinweights [numverts]
    [numattachments] [joint0] [weight0] ... [jointN] [weightN]
    ...
}

triangles [numtriangles] {
    [vertex0] [vertex1] [vertex2]
    ...
}

bindings [numjoints]
    matrix {
        [ax] [ay] [az]
        [bx] [by] [bz]
        [cx] [cy] [cz]
        [dx] [dy] [dz]
    }
    ...
}
```



### 2.1.2 New Classes/Members/Methods

- `Vertex`

- `Skin`

  - Drawing skin is independent of drawing a skeleton, it does not use `Cube->drawCube`. Instead, we should define its own variables (VAO, VBO, EBO…) and functions for rendering.

- `Joint`

  - Binding Matrix `B`

    $B=\left[\begin{array}{cccc}
    a_{1} & b_{1} & c_{1} & d_{1} \\
    a_{2} & b_{2} & c_{2} & d_{2} \\
    a_{3} & b_{3} & c_{3} & d_{3} \\
    0 & 0 & 0 & 1
    \end{array}\right]$

- `Skeleton`

  - `std::vector<Joint*> joints`， used for linking joints with skin when setting weights

## 2.2 Smooth Skin Algorithm

Skinning Equation:

- For vertex positions:

  $\mathbf{v}^{\prime}=\sum w_{i} \mathbf{W}_{i} \cdot \mathbf{B}_{i}^{-1} \cdot \mathbf{v}$

- For vertex normals (assume there’s no shearing or non-uniform scaling, thus not using inverse transpose):

  $\mathbf{n}^{*}=\sum w_{i} \mathbf{W}_{i} \cdot \mathbf{B}_{i}^{-1} \cdot \mathbf{n}$

  $\mathbf{n}^{\prime}=\frac{\mathbf{n}^{*}}{\left|\mathbf{n}^{*}\right|}$



## 2.3 Render Skin

Rendered with shading using at least two different colored lights.

### 2.3.1 Classic Lighting Model

> red book p361

```c++
color =
    // Ambient : simulates indirect lighting
    MaterialAmbientColor +
    // Diffuse : "color" of the object
    MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) ;
    // Specular : reflective highlight, like a mirror
    MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance);
```

`pow(cosAlpha,5)` is used to control the width of the specular lobe. Increase 5 to get a thinner lobe.

#### 2.3.1.1 Ambient Light

Ambient light is light not coming from any specific direction. The classic lighting model considers it a constant throughout the scene, forming a decent first approximation to the scattered light present in a scene.

It could either be accumulated as a base contribution per light source or be pre-computed as a single global effect.

The ambient light doesn’t change across primitives, so we will pass it in from the application as a `uniform` variable.

#### 2.3.1.2 Diffuse Light

Diffuse light is light scattered by the surface equally in all directions for a particular light source. Diffuse light is responsible for being able to see a surface lit by a light even if the surface is not oriented to reflect the light source directly toward your eye.

Computing it depends on the direction of the surface normal and the direction of the light source, but not the direction of the eye. It also depends on the color of the surface.

#### 2.3.1.3 Specular Light

Specular highlighting is light reflected directly by the surface. This highlighting refers to how much the surface material acts like a mirror. The strength of this angle-specific effect is referred to as shininess.

Computing specular highlights requires knowing the surface normal, the direction of the light source, and the direction of the eye.

## 2.4 Interaction

This program allows some way to interactively control individual DOF values in the skeleton.

## 2.5 Implementation on GPU

> [Implementing GPU skinning](https://subscription.packtpub.com/book/programming/9781800208087/10/ch10lvl1sec92/implementing-gpu-skinning)

This would most likely be done by just implementing it in the GLSL vertex shader. This requires storing the skinning data in the vertex buffer and passing the entire array of skeleton matrices to the vertex shader. (Alternately, one could implement this through other GPU techniques such as through Cuda.)

## 2.6 FUNCTIONS

**Currently implemented functions:**

1. Slider bar changes:
   1. Camera distance, azimuth, inclination;
   2. DOFs of every joint.
2. Press R to reset all DOF.
3. Skin at binding state
4. Attach skin to skeleton

**Technically feasible functions:**

1. // Texture 
2. // Change light

# 3. Animation

## 3.1 Load ANIM File

This program can load a keyframe animation from an `.anim` file and play it back on a skinned character.

### 3.1.1 ANIM File Description

The `.anim` file contains an array of channels, each channel containing an array of keyframes. The structure of the `.anim` file is as follows:

```json
animation {
    range [time_start] [time_end]
    numchannels [num]
    channel {
        extrapolate [extrap_in] [extrap_out]
        keys [numkeys] {
            [time] [value] [tangent_in] [tangent_out]
            ...
        }
    }
    channel {
       ...
    }
    ...
}
```

- `[time_start]` and `[time_end]` describe the time range in seconds that the animation is intended to play. This range doesn't necessarily correspond to the times of the first and last keyframes.

- number of channels `[num]` = 3 * number of joints + 3 (for the root translation). It’s also # poses
- The `channels` are listed with the <u>3 root translations first</u> in x,y,z order, f<u>ollowed by the x,y,z rotation channels for each joint</u> in **depth-first** order.

- The extrapolation modes `[extrap_in]` and `[extrap_out]` will be one of the following: `"constant"`, `"linear"`, `"cycle"`, `"cycle_offset"`, or `"bounce"`. 
- The keys themselves will be listed in increasing order based on their time. `[numkeys]` specifies the number of keyframes in the channel. Each key specifies its `[time]` and `[value]` in <u>floating point</u>. The tangent types `[tangent_in]` and `[tangent_out]` will be one of the following: `"flat"`, `"linear"`, `"smooth"`, or it will be an actual floating point slope value indicating the `fixed tangent mode`.

### 3.1.2 Animation Classes Structure

new classes should include: `AnimationPlayer`, `AnimationClip`, `Channel`, `Keyframe`

```c++
AnimationPlayer
|__AnimationClip
	|__Channel
	    |__Keyframe
```

- **AnimationPlayer**

  ```c++
  class AnimationPlayer {
      AnimationClip* clip;
  	// Need a skeleton to map the pose vector to a specific rig
  	Skeleton* skeleton;
  	float curTime, tStart, tEnd;
  	float deltaT = 0.01f; // 60fps, slow-motion video
  	std::vector<float> poses;
  	glm::mat4 rootTranslation;
  
  	AnimationPlayer(AnimationClip* Clip, Skeleton* Skeleton);
  	~AnimationPlayer();
  	void Update(); // evaluates current poses, set these poses, increments current time
  };
  ```

  - A pose is an array of values that <u>maps to a **rig**</u> (that’s why we need a skeleton).
  - If the rig contains only simple independent DOFs (instead of quarternions or other complex coupled DOFs) the pose can just be an array of floats with `size = # joints`.
  - 

- **AnimationClip**

  ```c++
  class AnimationClip { 
      float tStart, tEnd;
  	int numChannels;
  	std::vector<Channel*> channels;
  
  	AnimationClip();
  	~AnimationClip();
  	bool Load(const char* animfile);
  	void Precompute(); // each channel performs precomputation
  	void Evaluate(float time, std::vector<float>& pose); // passing pose vector by reference
  }; 
  ```

  - An animation clip will be stored as an array of `channels`.
  - The interface for accessing data (`Evaluate`) will be through a `pose` (Because we want to represent the whole pose of the character on each specific time/frame).

- **Channel**

  ```c++
  class Channel { 
      char extpIn[256], extpOut[256];
  	int numKeys;
  	std::vector<Keyframe*> keyframes;
  
  	Channel();
  	~Channel();
  	bool Load(Tokenizer* tknizer);
  	void Precompute(); // compute tangents & cubic coefficients
  	float Evaluate(float time);
  };
  ```

  - precompute coefficients

    $\left[\begin{array}{l}
    a \\
    b \\
    c \\
    d
    \end{array}\right]=\left[\begin{array}{cccc}
    2 & -2 & 1 & 1 \\
    -3 & 3 & -2 & -1 \\
    0 & 0 & 1 & 0 \\
    1 & 0 & 0 & 0
    \end{array}\right] \cdot\left[\begin{array}{c}
    p_0 \\
    p_1 \\
    \left(t_1-t_0\right)_{V_0} \\
    \left(t_1-t_0\right)_{V_1}
    \end{array}\right]$

- **Keyframe**

  ```c++
  class Keyframe { 
      float time;
  	float value;
  	float tanIn, tanOut; // tan values for fixed tangent mode
  	// Tangent rules to compute tan values that are not given, stored in char[]
  	char ruleIn[256], ruleOut[256]; 
  	float a, b, c, d; // Cubic coefficients
  
  	Keyframe();
  	~Keyframe();
  	bool Load(Tokenizer* tknizer);
  };
  ```

  - 

## 3.3 FUNCTIONS

**Currently implemented functions:**

1. Collapsible property panel;
2. Slider bar changes:

   1. Camera distance, azimuth, inclination;
   2. DOFs of every joint.
3. Press `R` to reset all DOFs.
4. Able to present:

   1. Only Skeleton (`.skel`)
   2. Undeformed Skin (Skin at binding state, `.skin`)
   3. Skeleton with attached skin (rig)
   4. Animated Rig
5. play controls

   1. Pause
   2. playback speed
   3. progress bar
   4. play mode (what to do after the end of the clip?), default is walking till the end of the world
      1. To infinity!
      2. Loop from start
      3. Stop at end
      4. Walk back and forth

**Technically feasible functions:**

1. // Change light according to time (simulate sunrise and sun set)

2. // Texture

3. // Textured ground

4. // Plot trajectory

# 4. Cloth Simulation

## 4.1 Overview

- This program simulates a piece of cloth made from particles, spring-dampers, and triangular surfaces. 
- It includes the effects of <u>uniform gravity, spring elasticity, damping, aerodynamic drag, and simple ground plane collisions</u>. 
- Some particles of the cloth are “fixed” in place in order to hold the cloth up. Simple controls are available to move these fixed points around.
- Controls to adjust the ambient wind speed are also provided.

## 4.2 Classes

```c++
|__Cloth
	|__SpringDamper; Rigid
    	|__Particle
```



### 4.1 Particle

```c++
class Particle {
public:
	float mass;
	bool isFixed;
	glm::vec2 texCoord;
	glm::vec3 position;
	glm::vec3 initPosition;
	glm::vec3 normal;
	glm::vec3 force;
	glm::vec3 acceleration;
	glm::vec3 velocity;

	Particle(glm::vec3 pos);
	~Particle();

	void AddForces(glm::vec3 f);
	void IntegrateMotion(float deltaT);
	void Reset();
};
```



To implement the ‘fixed’ particles, just add an additional `bool` to each Particle to indicate it’s fixed. Modify the `Particle::Update()` to do nothing if the `fixed bool` is true.

The fixed bool is set through the Cloth initialization process, so that you can experiment with fixing different parts of the cloth (such as an entire row of vertices or just the corners, etc.).

### 4.2 SpringDamper

```c++
class SpringDamper {
public:
	float restLen; // Spring rest length L0;
	float Ks; // Hooke coefficient, the spring constant describing the <stiffness> of the spring
	float Kd; // Damping constant
	Particle* p1, * p2;

	SpringDamper(Particle* particle1, Particle* particle2, float stiffness = 486.0f, float damping = 5.0f);
	~SpringDamper();

	void ComputeForce();
};
```



#### 4.2.1 Springs

Three types of springs: structural,  shear, bending.

![](https://i.imgur.com/doJrgf7.png)

Spring force by Hooke’s Law:
$$
{f}_{\text {spring }}=-k_s \mathbf{x}
$$
Where $k_s$ is the spring constant describing the <u>stiffness</u> of the spring, $\mathbf{x}$ is a vector describing the <u>displacement</u>. The spring force is therefore going to work against the displacement.

<img src="https://i.imgur.com/Uu6sKNA.png" style="zoom:50%;" />

#### 4.2.2 Dampers

Like a spring, a damper can connect between two particles. It will create a force along the line connecting the particles that <u>resists a difference in velocity</u> along that line.

Damping force:
$$
\mathbf{f}_{d a m p}=-k_d v_{c l o s e} \mathbf{e}
$$
where $k_d$ is the damping constant, $v_{\text {close }}=\left(\mathbf{v}_1-\mathbf{v}_2\right) \cdot \mathbf{e}$ is the closing velocity, and $\mathbf{e}$ is a unit vector that provides the direction.

### 4.3 Triangle

#### 4.3.1 Aerodynamic Force

Acts along the normal of the surface (triangle). The final force is assumed to apply over the entire triangle. We can simply apply 1/3 of the total force to each of the three particles connecting the triangle.

![](https://i.imgur.com/RZu7DPN.png)

![](https://i.imgur.com/ZPJHAIA.png)

![](https://i.imgur.com/S6zLTSi.png)

![](https://i.imgur.com/zk06FPA.png)

#### 4.3.2 Triangle Normals

The Triangles need to compute their normal every `Update` in order to do the physics/rendering.

In addition, one can implement dynamic smooth shading, where the normals are averaged at the vertices (Particles). A simple way to do this is:

- Loop through all particles and zero out the normal
- Loop through all triangles and add the triangle normal to the normal of each of the three particles it connects
- Loop through all the particles again and normalize the normal

### 4.4 Cloth

The `Cloth` class has one or more initialization functions that sets up the arrays and configures the connectivity. Different `init` functions could set up different <u>sizes</u> of cloth, different material <u>stiffness</u> properties, or different <u>configurations</u> like ropes, and more.

To do the user controls, add a control function to the `Cloth` that responds to keyboard presses and loops through all the particles and adjusts the position of only the fixed particles accordingly

## 4.3 Logic

```c++
specify initial conditions;
while (not finished) { 
    // Apply user interactions and other logic… 
    
    // Simulate 
    1. Compute all forces;
    2. Integrate motion;
    3. Collision response;
    
    // Draw or store results… 
}
```

## 4.5 FUNCTIONS

1. General :1st_place_medal:
   4. pause simulation
2. Cloth physics attributes & control :1st_place_medal:
   1. stiffness: structural, shear, bending
   2. damping: structural, shear, bending
   3. change pin mode & drop cloth:
      1. Pin Upper Corner
      2. Pin Upper Edge
      3. Drop Cloth
   4. move cloth
      1. translate +x (right): `D`
      2. translate -x (left): `A`
      3. translate +y (up): `W`
      4. translate -y (down): `S`
      5. translate -z (inward): `Q`
      6. translate +z (outward): `E`
      7. rotate clockwise: `C`
      8. rotate counterclockwise: `Z`
   5. Friction/elasticity with ground & sphere 
3. Wind :1st_place_medal:
   1. wind’s howling/calm
   2. move wind spawn
      1. Up: `W`
      2. Down: `S`
      3. Left: `A`
      4. Right: `D`
      5. Inward: `Q`
      6. Outward: `E`
   3. wind velocity (value)
4. Light :1st_place_medal:
   1. move light: 
      1. Up: `W`
      2. Down: `S`
      3. Left: `A`
      4. Right: `D`
      5. Inward: `Q`
      6. Outward: `E`

   2. change light color with hue wheel

5. Move camera :1st_place_medal:
   1. Up: `W`
   2. Down: `S`
   3. Left: `A`
   4. Right: `D`
   5. Inward: `Q`
   6. Outward: `E`
6. User grab :1st_place_medal:
   1. press mouse left button to drag the cloth
   2. quite grab mode either by right click mouse button or unclick the checkbox 
