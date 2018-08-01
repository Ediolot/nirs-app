# NIRS APP

**Note, comments in the source are a work in progress !**

[Windows installer x64](https://github.com/Ediolot/nirs-app/blob/master/installer/NirsAppInstaller.exe?raw=true)

This application is licensed under [CC BY-NC 4.0](https://creativecommons.org/licenses/by-nc/4.0/). This application's logo is licensed under [CC BY 3.0](https://creativecommons.org/licenses/by/3.0/) and made by [Freepik](https://www.freepik.com/) at [Flaticon](https://www.flaticon.com/).

## Project Summary

This application was development as a degree final project. It was developed under the supervision of <strong>Vanessa Munoz Cruz</strong> and <strong>Estefania Hernandez Martin</strong> by <strong>Jorge Sierra Acosta</strong>.<br><br>
The purpose of this project is the development of a multiplatform application to receive images taken by a near infrared spectroscopy hardware by the group of Neuroimaging and Neurochemistry of the ULL and build an interface in which the user may visualize and process such images.<br><br>
These images represent a bidimensional map of the changes in the oxygenation of brain tissues indicating an increment or decrease of the brain activity in that region relative to the initial reference point. The techniques used in the application may be applied to human or animal brain tissue.

## Abstract

_The purpose of this project is to development a multiplatform application which will be receiving images taken by a near infrared spectroscopy hardware by the group of Neuroimaging and Neurochemistry of the ULL and build an interface in which the user may visualize and process such images, applying if possible a statistical analysis.<br><br>
These images represent a bidimensional map of changes in the oxygenation of brain tissues indicating an increment or decrease of the brain activity in that region relative to the initial reference point. Even though the same technique can be applied to human brains, in this specific scenario the images were taken from a common rat one.<br><br>
Currently there are a variety of scripts codded for Matlab (mathematical software) which serve as a temporal solution to the problem to which this project pretends to offer a more robust one, creating a more intuitive and simpler interface grouping all functionality and removing the need to depend on the constant creation and modification of those scripts and the non-free licence needed for such software.<br><br>
Another objective is to explore the possibility to synchronize the new application with an Arduino in charge of sending a signal to the image capturing hardware indicating when the capturing of the basal is completed, this is, the reference image used to measure the different brain activations._

**[View full report](https://github.com/Ediolot/nirs-app/blob/master/doc/2-report-presentation/MemoriaTFG_JorgeSierraAcosta.pdf) (In spanish). A description of how the program works may be found in chapters 3 and 4. A [video](https://github.com/Ediolot/nirs-app/blob/master/doc/2-report-presentation/vidApp.mp4?raw=true) showing the program working.**

## Example data

Some data files provided by the group of Neuroimaging and Neurochemistry of the ULL are available to be tested in the program with images of rat brain tissue.

 - [experiment_1.exp](https://gitlab.com/Ediolot/nirs-app-data/raw/master/experiment_1.exp)
 - [experiment_2.exp](https://gitlab.com/Ediolot/nirs-app-data/raw/master/experiment_2.exp)

_Experiment 1 file was corrupted in the process even though it can still be loaded in the program is recommended to use experiment 2._

## Project structure

<table>
	<tr>
		<td>doc</td>
		<td>Images, briefings, and reports</td>
	</tr>
	<tr>
		<td>forms, src & resources</td>
		<td>QT source code, and web application</td>
	</tr>
	<tr>
		<td>installer</td>
		<td>Installer and configuration files for the QT installer framework</td>
	</tr>
	<tr>
		<td>logo</td>
		<td>Logo file and license</td>
	</tr>
	<tr>
		<td>original</td>
		<td>Original programs before the application was made</td>
	</tr>
</table>

## Dependencies
 - The installer was created using the QT Installer Framework.
 - [Eigen library](http://eigen.tuxfamily.org/index.php?title=Main_Page)
 - [DSPFilters](https://github.com/vinniefalco/DSPFilters)
 - Other technologies used may be found in the complete report.

**The QT file `NIRS_APP.pro` should be modified in order to include the path to those two libraries**, this is, changing the `INCLUDEPATH` to point to the Eigen unique header and DSPFilters header. It is also needed to add the source files of the DSPFilters library to the `SOURCES` variable, as they are not packaged into a separated library.

## TODOs

- [ ] A settings file to save and load the user settings each time the program starts.
- [ ] Load basal image from a file.
- [ ] Enable ROI in both images.
- [ ] T-student analysis.
- [ ] Short menu when there is no space available for the main section.
- [ ] Better multithread protection in the C++ code.
