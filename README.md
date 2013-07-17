# CVDebug

A helper debugging tool for any application using the [OpenCV library](http://opencv.org/).

## Motivation

The motivation for this tool is the inability to simultaneously debug your application and view visual information in OpenCV, such as images - while your application is paused in the debugger, the only way to display the data is through hexadecimal memory dump. If you manage to display the image into a window (like through `imshow`), you lose the ability to show the image and work with it once you pause the debugger.

A project with similar goal was started alongside this project for the Visual Studio - [Image Watch](http://visualstudiogallery.msdn.microsoft.com/e682d542-7ef3-402c-b857-bbfba714f78d), showing the necessity for such tool. Our approach is better, because it is not bound to a specific IDE or an operating system - it is truly multiplatform.

## Overview

This tool consist of a helper library, which you link into your application, and a separate visualization application. These two components communicate over [ZeroMQ](http://www.zeromq.org/) and allow the transfer of images from the library to the application. If the CVDebugger application is not running, the library acts as if nothing happened, allowing you to run your application "without debug mode".

    +-----------------------+         +-------------------+
    |    Your application   |         |     CVDebugger    |
    |-----------------------|         |-------------------|
    | +-------------------+ |         | +---------------+ |
    | |      OpenCV       | |    +--->| |  Saved images | |
    | +-------------------+ |    |    | |---------------| |
    |                       | ZeroMQ  | |               | |
    | +-------------------+ |    |    | |               | |
    | |      CVDebug      +------+    | |               | |
    | +-------------------+ |         | +---------------+ |
    +-----------------------+         +-------------------+

## Usage

The application stores a number of images in memory in a circular buffer and shows you a list of images with the following info:

* name of the image - coming from your application
* the resolution of the image
* the element type and number of channels

When you click on an image thumbnail in the list, a full view of the image appears in the main area, allowing you to zoom in on the image and view individual pixel data. 

## Debugging API

The library `cvdebug` has a very simple API - it provides only the following function:

    void cvdebug_image(const cv::Mat &image, const std::string &name);

If the CVDebugger application is started, the given image is sent to the visualizer and its thumbnail appears in the list along with the name you specified. If it is not started, nothing happens. In any case, this function does not block.

You can format the name of the image to give additional information, for example:

    cvdebug_image(myimage, format("My image %d / %d", i, total));

The `format` function is provided by the OpenCV library.

## License

Copyright (C) 2013 Matej Šuster, Michal Kottman

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
