# 3. Animation

## 3.1 Load File

This program can load a keyframe animation from an `.anim` file and play it back on a skinned character.

ANIM File Description

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

## 3.2 Animation Classes Structure

new classes should include: `AnimationPlayer`, `AnimationClip`, `Channel`, `Keyframe`, `AnimRig`

```c++
AnimationPlayer
|__AnimationClip
	|__Channel
	    |__Keyframe
|__AnimRig
    |__Skeleton
    	|__Joint
    		|__Cube
    		|__DOF
    |__Skin
    	|__Vertex
    	|__Skeleton
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


## 3.3 Features

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

**Technically feasible functions (to-dos):**

1. // Change light according to time (simulate sunrise and sunset) or user input

2. // Textured character

3. // Textured ground

4. // Plot trajectory