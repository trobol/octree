# octree

This is the repository for our Advanced Realtime rendering course. In this project we looked to create high fidelity ray-traced sparse voxel octrees and be able to animate them efficiently.
We were unsuccesful but given more time and more reseaerch about managing large chunks of memory, this could work extremely well. 

Method:
Using a method presented in a paper "Efficient Sparse Voxel Octrees", we were able to achive optimized raytraced octrees. Then using this we implemented storing those octrees and the ability to use them as keyframes, where they were interpolated if possible.we attempted to animate the the structures using two techniques working together.
The first part of is to pre-calculate frames of the animation and bake them into SVOs that can be swapped out at run
time. This swapping will not create a smooth animation, especially as the delta between the frames grows bigger. To
mitigate this, we interpolate the part of the structure that can be interpolated, the contours. In order to perform this
interpolation, the contours are stored in a texture instead of within the same buffer as the voxel data. The contour data
for each frame is stored along one row, this allows us to sample the y coordinate based on the frame and the offset into
the next frame. The GPU will interpolate between the contours stored on each frame, to allow for a smoother transition
between individual frames, but there are some limitations. Contours do not extend beyond the bounds of a voxel when
rendered meaning there is a hard limit to the extent that a voxel can grow or shrink.
