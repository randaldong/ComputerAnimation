# 2. Skinning

## 2.1 Load File

The program can load any `.skin` and `.skel` file given to it.

It also works if only one of the files is given. For example, if only the `.skel` file is given, it should operate as in [Skeleton.md](./Skeleton.md). If only the skin file is given, it should display the skin mesh in its undeformed state (i.e., in its original binding space).

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



The `.skin` file contains arrays of vertex data (including positions, normals, and joint weights), an array of triangle data, and an array of binding matrices.

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

## 2.2 New Classes/Members/Methods

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

## 2.3 Smooth Skinning

![](https://i.imgur.com/6bapMvc.png)

![](https://i.imgur.com/bSB5GNF.png)

![](https://i.imgur.com/UUkDlmD.png)

![](https://i.imgur.com/w2sKFPW.png)

![](https://i.imgur.com/jEsZHyz.png)

![](https://i.imgur.com/Vp7zTVL.png)

Skinning Equation:

- For vertex positions:

  $\mathbf{v}^{\prime}=\sum w_{i} \mathbf{W}_{i} \cdot \mathbf{B}_{i}^{-1} \cdot \mathbf{v}$

  - $\mathbf{v}$ is the original vertex position in the binding space
  - $\mathbf{v}^{\prime}$ is the transformed vertex position in world space
  - $w_i$ is the weight of the vertex attachment to joint $i$ (note $\sum w_i=1$ )
  - $\mathbf{W}_i$ is the world matrix of joint $i$
  - $\mathbf{B}_i$ is the binding matrix for joint $i$

- For vertex normals (assume there’s no shearing or non-uniform scaling, thus not using inverse transpose):

  $\mathbf{n}^{*}=\sum w_{i} \mathbf{W}_{i} \cdot \mathbf{B}_{i}^{-1} \cdot \mathbf{n}$

  $\mathbf{n}^{\prime}=\frac{\mathbf{n}^{*}}{\left|\mathbf{n}^{*}\right|}$

## 2.4 Blinn-Phong Illumination Model

Rendered with shading using at least two different colored lights.

> red book p361

> Uses the halfway vector. Faster to compute than Phong’s reflection vector. Still view-dependent since H depends on V

- use angle between the halfway vector and the surface normal
- use cos to measure if they are pointing in the same direction
- use shininess as the power to control wideness of the highlight

**Blinn-Phong’s specular term:**

- `float spec = pow(max(dot(halfVec, normal), 0.0), material.shininess);`
- `vec3 specular = light.specular * (spec * material.specular);`

![](https://i.imgur.com/Z0phatN.png)

![](https://i.imgur.com/Rk7WKWt.png)

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

### 2.4.1 Ambient Light

Ambient light is light not coming from any specific direction. The classic lighting model considers it a constant throughout the scene, forming a decent first approximation to the scattered light present in a scene.

It could either be accumulated as a base contribution per light source or be pre-computed as a single global effect.

The ambient light doesn’t change across primitives, so we will pass it in from the application as a `uniform` variable.

### 2.4.2 Diffuse Light

Diffuse light is light scattered by the surface equally in all directions for a particular light source. Diffuse light is responsible for being able to see a surface lit by a light even if the surface is not oriented to reflect the light source directly toward your eye.

Computing it depends on the direction of the surface normal and the direction of the light source, but not the direction of the eye. It also depends on the color of the surface.

### 2.4.3 Specular Light

Specular highlighting is light reflected directly by the surface. This highlighting refers to how much the surface material acts like a mirror. The strength of this angle-specific effect is referred to as shininess.

Computing specular highlights requires knowing the surface normal, the direction of the light source, and the direction of the eye.

## 2.5 Interaction

This program allows some way to interactively control individual DOF values in the skeleton.

## 2.6 Features

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

## Appendix: Implementation on GPU

> [Implementing GPU skinning](https://subscription.packtpub.com/book/programming/9781800208087/10/ch10lvl1sec92/implementing-gpu-skinning)

This would most likely be done by just implementing it in the GLSL vertex shader. This requires storing the skinning data in the vertex buffer and passing the entire array of skeleton matrices to the vertex shader. (Alternately, one could implement this through other GPU techniques such as through Cuda.)