# MozJpegGUI

 This software is a high-speed image-file-to-jpeg-converter which can handle massive number of image files with high-quality and high-compression rate. Main point of the software is its multi-thread capability. When you are converting several hundreds image files, this makes a big difference to other software which converts one file at a time.

## Demo
Example of converting 450 png files into jpeg files:

https://youtu.be/1yxq6xexGdE

## Feature
 a) This software converts an image files to jpeg files by using mozjpeg library; which enables high-compression and high-quality jpeg compression developed by Mozilla JPEG Encoder Project.
 
 b) The software is capable to run multiple conversion at same time. As a result, the conversion of multiple files is very fast when used on multi-core processors.
 
 c) You can save multiple conversion settings. So, you can modify the settings which fits to your purpose easily by simply selecting stored settings.
 
 d) The software can handle following input images:
    .bmp, .gif, .jpg, .jpeg, .png, .ppm, .tga
    
 e) You can add an image file by simply drag-and-drop the files, or by pushing “Add from folder” button to add all image files from a folder.
 
 f) In some cases, the converted files might have larger file size. By checking “Use Smaller” checkbox, you can let the software cancel the conversion of the file and let it copy the source file into the result folder. This decision will be done for each individual file. The situation can happen when the source file was a monochrome file. As JPEG files are not capable for monochrome images, mozjpegGUI will convert them into grayscale images. As a result, the file size might increase.
 
 g) Sorry for the people who uses ultra-high-spec CPUs, number of the logical CPU cores which the software can handle is expected to 64 due to Windows OS limitation. Some special treatment should be done in the software to avoid this limitation, but because such CPUs are expensive and I cannot buy and test them, there is no schedule for the software to implement such treatments.
 
 h) There are 32 bit version and 64 bit version of the software. The 32 bit version can work on both Windows 10 32 bit / 64 bit versions. The 64 bit version can work only on Windows 10 64 bit version, but seems to be about 8% faster than 32 bit version.

## Requirement
Intel / AMD based PC, Windows 10. No runtimes required.

## Usage
 a) Execute MozJpegGUI.exe
 
 b) Drag-and-drop the files which you want to convert into white area of the software. Or, click “Add from folder” button and select the folder which includes the files which you want to convert. Note: on selecting the folder, files does not appear in the dialog box. This is a specification of the Windows.
 
 c) Click “Browse” button and select folder to output the converted files.
 
 d) Change conversion settings by checking “Use Smaller” checkbox, “Overwrite” checkbox, Quality, and so on. These settings are stored when you close the software or when you change setting set from “Manage settings”.
 
 e) If you are ready to convert, press “Convert” button and start conversion.
 
 f) A progress dialog appears. Wait until its progress becomes 100%. In the dialog, the “Input buffer” shows amount of input files which are read into memory, the “CPU” shows number of conversions which are running. If both “Input buffer” and “CPU” are close to 0, then the bottleneck is reading of the file from storage. If CPU is close to 100%, then the bottleneck is CPU; which means the computer is working at its full spec.
 
 g) When the conversion is completed, a message showing the completion will be shown.
 
 h) If you want to continue the conversion with other sets of the files, push “Clear” button, and start again from b).
 
## Install
 Uncompress the files into any folder you want. By double-clicking MozJpegGUI.exe, you can start the software. The software does not have installer at this moment.

## Licence
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
    PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    This software is based in part on the work of the Independent JPEG Group.

    As this software uses mozjpeg, libpng and zlib libraries, sub-module of
    the software inherites their licenses as shown in license.md.
