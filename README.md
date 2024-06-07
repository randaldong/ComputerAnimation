# Computer Animation

This documentation aims to provide necessary knowledge for a smooth handover of the Computer Animation project, including project knowledge, technical knowledge, resources, communication, and others. To avoid lengthy, text-heavy documents that could overwhelm the readers, details of individual modules are covered in separated files as follows, and some sections in those files will be referenced in this documentation.

1. [Skeleton.md](./Skeleton.md)
2. [Skinning.md](./Skinning.md)
3. [Animation.md](./Animation.md)
4. [ClothSimulation.md](./ClothSimulation.md)

## 1. Project Knowledge

This project aims to build an engine that enables real-time character animations and simulations, as well as  interactive user controls for them. Currently, it includes two basic modules: character animation and cloth simulation.



### 1.1 Character Animation

#### 1.1.1 Overview

The character animation module is made up of Skeleton, Skin, and Animation. The final project allows switching between different views and control the animation clip.



#### 1.1.2 Feature Status

A wide range of features have been implemented, including:

- load character skeleton, skin, and animation from `.skel`, `.skin`, and `.anim` files (essentially JSON-like nested strings) and display them in 3D views.

- able to present:

  - skeleton only, rendered as boxes
  - undeformed skin at binding state
  - skeleton after smooth skinning
  - a short animation clip of this character

- collapsible property panel for interactive user control:

  - use slider bar to change:

    1. Camera distance, azimuth, inclination;
    2. DOFs of every joint.
  - press `R` to reset all DOFs.

  - animation controls:
    - pause
    - playback speed
    - progress bar
    - play mode (what to do after the end of the clip?), default is walking till the end of the world
      1. To infinity!
      2. Loop from start
      3. Stop at end
      4. Walk back and forth



The following features are milestones to be achieved (refer to [JIRA](https://randaldong.atlassian.net/jira/software/c/projects/AN/issues?jql=project%20%3D%20%22AN%22%20ORDER%20BY%20created%20DESC) for more details):

- support for various input file formats, estimated time: 2 weeks, due date: Jun 21, 2024, priority: high; 
- textured character, estimated time: 1 week, due date: Jun 21, 2024, priority: high; 
- textured ground/scene, estimated time: 1 day, due date: Jun 21, 2024, priority: medium; 
- light control, estimated time: 1 week, due date: Jul 05, 2024, priority: low; 
- animation blend, estimated time: 1 week, due date: Jul 05, 2024, priority: low; 



#### 1.1.3 Challenges & Solutions





### 1.2 Cloth Simulation

#### 1.2.1 Overview

The cloth simulation module uses a **Mass-Spring System** to simulate a piece of cloth.



#### 1.2.2 Feature Status

The cloth simulation engine currently supports following features:

1. General
   4. pause simulation
2. Cloth control & physics attributes
   1. move cloth
      1. translate +x (right): `D`
      2. translate -x (left): `A`
      3. translate +y (up): `W`
      4. translate -y (down): `S`
      5. translate -z (inward): `Q`
      6. translate +z (outward): `E`
      7. rotate clockwise: `C`
      8. rotate counterclockwise: `Z`
   2. stiffness: structural, shear, bending
   3. damping: structural, shear, bending
   4. change pin mode & drop cloth:
      1. Pin Upper Corner
      2. Pin Upper Edge
      3. Drop Cloth
   5. Friction/elasticity with ground & sphere 
3. Wind control
   1. wind’s howling/calm
   2. move wind spawn
      1. Up: `W`
      2. Down: `S`
      3. Left: `A`
      4. Right: `D`
      5. Inward: `Q`
      6. Outward: `E`
   3. wind velocity (value)
4. Light control
   1. move light: 
      1. Up: `W`
      2. Down: `S`
      3. Left: `A`
      4. Right: `D`
      5. Inward: `Q`
      6. Outward: `E`

   2. change light color with hue wheel
5. Move camera
   1. Up: `W`
   2. Down: `S`
   3. Left: `A`
   4. Right: `D`
   5. Inward: `Q`
   6. Outward: `E`
6. User grab
   1. hold mouse left button to drag the cloth
   2. quit grab mode either by right click mouse button or unclick the checkbox 



Milestones to be achieved in the coming months (refer to [JIRA](https://randaldong.atlassian.net/jira/software/c/projects/AN/issues?jql=project%20%3D%20%22AN%22%20ORDER%20BY%20created%20DESC) for more details):

- try optimization methods for both realistic considerations and performance, estimated time: 4 weeks, due date: Jul 5, 2024, priority: low



#### 1.2.3 Challenges & Solutions





## 2. Technical Knowledge

### 2.1 Tech Stack

- IDE:
  - For Windows: Visual Studio or Visual Studio Code
  - For Mac: Xcode or Visual Studio Code
- Dependencies:
  - GLM: https://github.com/g-truc/glm
  - GLFW: https://www.glfw.org/
  - GLAD: https://github.com/Dav1dde/glad
  - ImGui: https://github.com/ocornut/imgui
  
- Version control: [GitHub](https://github.com/randaldong/ComputerAnimation.git)
- Bug tracking: [JIRA](https://randaldong.atlassian.net/jira/software/c/projects/AN/issues?jql=project%20%3D%20%22AN%22%20ORDER%20BY%20created%20DESC), Radar, GitHub Issues, Slack…

> Note: In real-life situations, companies may require developers to use various (often internal) tools for version control and bug tracking.



### 2.2 Architecture Design

1. Classes & Hierarchy: see [Miro](https://miro.com/welcomeonboard/OExBODFHQVN5V0hSZlNKVk54ZHFEdHhqRGNabElJVVNzNTJyTTNsdkNBRjFLTnQwVXRxUmRhRmx5SG5idkxTb3wzNDU4NzY0NTQ0NTg1ODk1MDgwfDI=?share_link_id=393816811085)
2. Data Files
   - `.skel` file, refer to [1.1 Load File in Skeleton.md](./Skeleton.md#11-load-file)

   - `.skin` file, refer to [2.1 Load File in Skinning.md](./Skinning.md#21-load-file)

   - `.anim` file, refer to [3.1 Load File in Animation.md](./Animation.md#31-load-file)



### 2.4 Key Algorithms

- Skeleton Posing: refer to [1.2 Skeleton Posing in Skeleton.md](./Skeleton.md#12-skeleton-posing) 
- Smooth Skinning: refer to [2.3 Smooth Skinning in Skinning.md](./Skinning.md#23-smooth-skinning) 
- Blinn-Phong Illumination Model: refer to [2.4 Blinn-Phong Illumination Model in Skinning.md](./Skinning.md#24-blinn-phong-illumination-model)

- Mass-Spring System: refer to [4.2 Mass-Spring System in ClothSimulation.md](./ClothSimulation.md#42-mass-spring-system)



### 2.5 Deployment

- **Installation**

  - For Windows:

    - Visual Studio (tested on [VS 2022](https://visualstudio.microsoft.com/vs/))

    - Git/GitHub Desktop

  - For Mac:

    - Xcode
    - Git/GitHub Desktop

> Note: This application has not been tested on Mac and Linux, but the installation steps should be similar as long as all dependencies are included and referenced correctly.



- **Running**



### 2.6 Development

- Coding Standards

- Version Control: Git/GitHub

- Bug Tracking: JIRA, Radar, GitHub Issues, Slack…

## 3. Resources

- API documentations:
  - OpenGL: https://registry.khronos.org/OpenGL-Refpages/gl4//
  - GLM: https://glm.g-truc.net/0.9.9/api/modules.html
  - ImGui: https://github.com/ocornut/imgui
- Tutorials:
  - [LearnOpenGL](https://learnopengl.com/)
  - YouTube playlist [Introduction to OpenGL](https://www.youtube.com/playlist?list=PLvv0ScY6vfd9zlZkIIqGDeG5TUWswkMox) by [Mike Shah](https://www.youtube.com/@MikeShah)
  - YouTube playlist [Skeletal Animation](https://www.youtube.com/playlist?list=PLA0dXqQjCx0QY7V0UOSaMcKv1BHA_LzPf) by [OGLDEV](https://www.youtube.com/@OGLDEV)
- Domain knowledge:
  - Mathematics for 3D Game Programming and Computer Graphics, Third Edition 3rd Edition
  - 3D Math Primer for Graphics and Game Development 2nd Edition
  - Fundamentals of Computer Graphics 4th Edition
  - Real-Time Rendering, Fourth Edition 4th Edition
  - Physically Based Rendering, fourth edition: From Theory to Implementation 4th Edition
  - OpenGL Programming Guide 9th Edition
  - Physics for Game Programmers 2nd Edition
  - Computer Animation 3rd Edition
  - UCSD CSE 169 Computer Animation lecture slides: https://cseweb.ucsd.edu/classes/wi20/cse169-a/sessions.html

## 4. Communication

- Email (or Apple mail, if I have one): randaldongdr@gmail.com
- Slack: message, video meetings
- iMessage: *858-205-3489*

- Phone call: *858-205-3489*
- Pair programming (if necessary): [Tuple](https://tuple.app/), [Visual Studio Live Share](https://visualstudio.microsoft.com/services/live-share/), [VS Code Live Share](https://code.visualstudio.com/learn/collaboration/live-share), etc.

## 5. Misc.

Guidance on how to write handover documentations for future need:

1. [handover-documentation-checklist.md](https://gist.github.com/TimothyJones/1508a7081405d57073b99180312f5caa)
2. [handover.md](https://github.com/dwarvesf/playbook/blob/master/engineering/handover.md)
