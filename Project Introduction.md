# 1. Character Animation

Hello everyone and welcome to my channel! In this video, I will show you one of my latest projects, my self-developed engine for character animation. 



This engine offers a wide range of features. It can load character skeleton, skin, and animation from .skel, .skin, and .anim files and display in 3D. It’s able to present skeleton rendered as boxes, undeformed skin at binding state, skeleton after smooth skinning, and a short animation clip of this character. 



I used ImGUI to develop a collapsible property panel that allows interactive parameter adjustment. You can use the drop-down menus to choose the skeleton you want to load. Once you’ve made your choice, the camera will be automatically set to a suitable status for viewing.

The slide bars allow you to change camera settings such as distance, azimuth, and inclination. You can also adjust DOFs of each joint. Also, if you want to start all over again, just press R to reset all DOFs.



The animation function plays a clip of a skinned character, which is a walking wasp. You can pause the clip, change the playback speed, and move the progress bar to wherever you want. Besides, you can choose the play mode, which means you can tell the engine what to do if the clip has reach the end. There are four choices available: "To infinity!", "Loop from start", "Stop at end", "Walk back and forth". "To infinity!" means this little wasp will keep walking as if this animation never reaches its end. "Loop from start" is just replay this clip from its beginning, and "Stop at end" is simply stopping when the time’s out. "Walk back and forth" means that when the wasp has come to the end of this animation, it will walk back the same way it came and keep doing that in this endless loop.



The implementation is straightforward. The first part is data parsing. The given files are JSON-like nested hierarchical text files. The `.skel file` is a hierarchy of joints. Each joint lists some relevant data about its configuration. The `.skel` data file has 8 keyword tokens which specify properties of a particular joint and are followed by data. The .skin file contains arrays of vertex data, an array of triangle data, and an array of binding matrices. The `.anim` file contains an array of channels, each channel containing an array of keyframes. I used a string tokenizer to parse and load the data. Then, I created classes for rigging, animation. For rigging, the hierarchy is: Skin-Vertex, Skeleton-Joint-DOF; and animation has the hierarchy of AnimationPlayer, AnimationClip, Channel, and Keyframe. For rendering, I used classic Blinn–Phong reflection model which includes ambient, diffuse, and specular terms.



That’s all for my character animation engine, thank you for watching. I look forward to sharing more about my projects and other cool stuff that I’ve been working on with you in the future. Don't forget to like, share, and subscribe to stay up-to-date with all of my latest content!

# 2. Cloth Simulation

Hey everyone, welcome back to my channel! Today, I want to share with you a project that I've been working on for the past 2 weeks, it’s called Randal’s Magic Cloth.

This project is a cloth simulation engine. It simulates a piece of cloth made from particles, three types of spring-dampers, and triangular surfaces. It includes the effects of uniform gravity, spring elasticity, damping, aerodynamic drag on triangular surfaces, and cloth collisions with ground and sphere.

The powerful dear imgui was used to develop the half-transparent and collapsible control panel. The engine enables various user controls. Generally, you can see the simulation FPS on the top, and pause the simulation by clicking the “pause” checkbox. The cloth, light, wind spawn, and camera are all movable and their moving key map is W for up, S for down, A for left, D for right, Q for inward, and E for outward. For cloth, apart from just translating it, you can also rotate is clockwise by pressing C and rotate it counterclockwise by pressing Z. Remember that before moving them, you have to click the corresponding checkbox so that you move the right object. 

This engine is highly customizable, you can also load different textures for cloth, ground, sphere, and wind spawn. Currently, cloth texture can be chosen from The Starry Night by Vincent Van Gogh , Allegory of Sight by Jan Brueghel the Elder and Peter Paul Rubens, and wallpapers of World of Warcraft, The Witcher 3, God of War: Ragnarok, and Genshin Impact. 

Other controls of the cloth including changing cloth pin mode between “pin upper corner” and “pin upper edge”; changing draw mode among “draw particles”, “draw strings”, and “draw cloth”; changing the Hooke coefficient and damping constant of structural, shearing, and bending springs; and changing the friction with ground and sphere.

The light in the scene is represented as a tiny sphere. You can adjust it’s color by picking one in the hue wheel, and the color will be used to render this light bulb.

Wind is blowing from the wind spawn to the position of the cloth, which is the direction of wind speed. Initially, wind’s velocity value is close to zero. If you click on the button “Wind’s Howling”, you will be able to adjust it’s velocity value by the slider bar. If you want to stop the wind, just click on the button “Wind’s Calm”.

Another interesting function is user mouse grab. After you clicking the checkbox to activate grab mode, you and drag the cloth with your mouse and the cloth will move the way you drag it.

That’s all for my magic cloth project, thanks for watching! If you think this project is interesting, please like and subscribe, that means a lot to me, thank you so much!