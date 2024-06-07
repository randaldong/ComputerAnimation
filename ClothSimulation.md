# 4. Cloth Simulation

## 4.1 Overview

- This program simulates a piece of cloth made from particles, spring-dampers, and triangular surfaces. 
- It includes the effects of <u>uniform gravity, spring elasticity, damping, aerodynamic drag, and simple ground plane collisions</u>. 
- Some particles of the cloth are “fixed” in place in order to hold the cloth up. Simple controls are available to move these fixed points around.
- Controls to adjust the ambient wind speed are also provided.

## 4.2 Mass-Spring System

```c++
Cloth
    |__spring-dampers (structural, shearing, bending)
        |__2 particles each
    |__triangles
        |__3 particles each
```

<img src="https://i.imgur.com/yOAKStT.png" alt="yOAKStT.png|314" style="zoom:30%;" />

### 4.2.1 Particle

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

### 4.2.2 SpringDamper

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

#### 4.2.2.1 Springs

Three types of springs: structural,  shear, bending.

![](https://i.imgur.com/doJrgf7.png)

Spring force by Hooke’s Law:
$$
{f}_{\text {spring }}=-k_s \mathbf{x}
$$
Where $k_s$ is the spring constant describing the <u>stiffness</u> of the spring, $\mathbf{x}$ is a vector describing the <u>displacement</u>. The spring force is therefore going to work against the displacement.

<img src="https://i.imgur.com/Uu6sKNA.png" style="zoom:50%;" />

#### 4.2.2.2 Dampers

Like a spring, a damper can connect between two particles. It will create a force along the line connecting the particles that <u>resists a difference in velocity</u> along that line.

Damping force:
$$
\mathbf{f}_{d a m p}=-k_d v_{c l o s e} \mathbf{e}
$$
where $k_d$ is the damping constant, $v_{\text {close }}=\left(\mathbf{v}_1-\mathbf{v}_2\right) \cdot \mathbf{e}$ is the closing velocity, and $\mathbf{e}$ is a unit vector that provides the direction.

### 4.2.3 Triangle

#### 4.2.3.1 Aerodynamic Force

Acts along the normal of the surface (triangle). The final force is assumed to apply over the entire triangle. We can simply apply 1/3 of the total force to each of the three particles connecting the triangle.

![](https://i.imgur.com/RZu7DPN.png)

![](https://i.imgur.com/ZPJHAIA.png)

![](https://i.imgur.com/S6zLTSi.png)

![](https://i.imgur.com/zk06FPA.png)

#### 4.2.3.2 Triangle Normals

The Triangles need to compute their normal every `Update` in order to do the physics/rendering.

In addition, one can implement dynamic smooth shading, where the normals are averaged at the vertices (Particles). A simple way to do this is:

- Loop through all particles and zero out the normal
- Loop through all triangles and add the triangle normal to the normal of each of the three particles it connects
- Loop through all the particles again and normalize the normal

### 4.2.4 Cloth

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

- Simulation components: particle, spring-damper, surfaces...
- Simulation stability:
    - Forward Euler integration with small time steps/adaptive time steps/oversampling
    - Backward Euler integration
    - Other integration methods
- Simulation process:
    - Compute Forces
        - For each particle: apply gravity
        - For each spring-damper: compute & apply spring-damper forces
        - For each triangle: compute & apply aerodynamic forces
    - Integrate Motion
        - For each particle: compute acceleration and apply forward Euler integration

    - Collision detection & response
    - Apply Constraints
       - For each particle: if intersecting, push to legal position & adjust velocity

## 4.5 Features

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
   1. hold mouse left button to drag the cloth
   2. quit grab mode either by right click mouse button or unclick the checkbox 