### Description

In this project i made a simple robot that will track the movement of designated object, in this case was a ball, and moved itself following the object. The ball used in this project is a tennis ball with solid color, made it easy to be identified. To drive the motor, i used L298 driver motor connected with the GPIO from the raspberry pi. I used opencv library for image processing and wiringpi to control the GPIO from raspberry. Block diagram of the component used in this project can be seen below. 

<p align="center">
  <img  src="https://github.com/falithurrahman/ball_tracking_robot/blob/master/block_diagram.jpg">
</p>

The camera, Logitech C270, will capture video frame to detect  position of the ball. The frame size is reduced smaller than the original frame in order to ease the computational work of raspberry pi. Next i perform basic thresholding and morphological filter operation to the frame until i get the binary image of the frame. Then i use *find_contour* function and extract the moment (centroid) from contour of the ball. I compare the moment (centroid) of this contour with the middle of the frame. The illustration of what process this robot does inside it's mind is available below.

<p align="center">
  <img  src="https://github.com/falithurrahman/ball_tracking_robot/blob/master/pos_diff.JPG">
</p>

<p align="center">
  <img  src="https://github.com/falithurrahman/ball_tracking_robot/blob/master/neg_diff.JPG">
</p>

If the difference between moment and middle of the frame is negative, i suppose the ball is at the left side of the robot, and vice versa. To make the robot follow the ball, i made the difference between the moment and middle of the frame is close to 0. I use a simple if else trick to make it works. You can see the simple flowchart to make this robot works below.

<p align="center">
  <img  src="https://github.com/falithurrahman/ball_tracking_robot/blob/master/flowchart.jpg">
</p>