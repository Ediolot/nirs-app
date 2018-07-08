function varargout = nirscam(varargin)
% NIRSCAM MATLAB code for nirscam.fig
%      NIRSCAM, by itself, creates a new NIRSCAM or raises the existing
%      singleton*.
%
%      H = NIRSCAM returns the handle to a new NIRSCAM or the handle to
%      the existing singleton*.
%
%      NIRSCAM('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in NIRSCAM.M with the given input arguments.
%
%      NIRSCAM('Property','Value',...) creates a new NIRSCAM or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before nirscam_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to nirscam_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help nirscam

% Last Modified by GUIDE v2.5 16-Jun-2016 10:31:35

global width height bitpix dark gain data time sampleFreq 

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @nirscam_OpeningFcn, ...
                   'gui_OutputFcn',  @nirscam_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before nirscam is made visible.
function nirscam_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to nirscam (see VARARGIN)

% Choose default command line output for nirscam
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes nirscam wait for user response (see UIRESUME)
% uiwait(handles.figure1);

% --- Outputs from this function are returned to the command line.
function varargout = nirscam_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on mouse press over axes background.
function imageAxesClick(hObject, eventdata, handles)
% hObject    handle to imageAxes (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
axesHandle  = get(hObject,'Parent');
coordinates = get(axesHandle,'CurrentPoint'); 
coordinates = round(coordinates(1,1:2));
process(coordinates(2), coordinates(1), findall(0, 'tag', 'l1Axes'), findall(0, 'tag', 'l2Axes'));

%message = sprintf('x: %.1f , y: %.1f',coordinates (1) ,coordinates (2));
%helpdlg(message);

% --- Executes on button press in loadPushButton.
function loadPushButton_Callback(hObject, eventdata, handles)
% hObject    handle to loadPushButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global width height bitpix dark gain data time sampleFreq

[filename, pathname, ~] = uigetfile('*.exp', 'Selección de fichero de experimento');
if (~isequal(filename,0) && ~isequal(pathname,0))
    [width, height, bitpix, dark, gain, timestamps, data] = loadExperimentFile(fullfile(pathname, filename));
    time = (timestamps - timestamps(1)) / 1000000000.0;
    sampleFreq = 1.0 / mean(time(2:size(time)) - time(1:size(time)-1));
    axes(handles.imageAxes);
    h = imshow((cast(data(1:width/2,:,1), 'double') - dark(1:width/2,:)) .* gain(1:width/2,:));
    set(h, 'ButtonDownFcn', @imageAxesClick);
end


function process(i, j, chartHandle1, chartHandle2)
global data dark gain width time filterCoeff
 selection = (24 < time) & (time < 287);
 start_selection = find(selection, 1, 'first');
 end_selection = find(selection, 1, 'last');

% Seleccion del entorno del punto para las dos long de onda a lo largo del
% tiempo
l1data = cast(data(i-1:i+1,j-1:j+1,:), 'double');
l2data = cast(data((i-1)+(width/2):(i+1)+(width/2),j-1:j+1,:), 'double');
% Repeticion del dark para poder sustraerlo
l1dark = repmat(dark(i-1:i+1,j-1:j+1,:), 1, 1, size(l1data, 3));
l2dark = repmat(dark((i-1)+(width/2):(i+1)+(width/2),j-1:j+1,:), 1, 1, size(l1data, 3));
% Repeticion del gain para poder multiplicarlo
l1gain = repmat(gain(i-1:i+1,j-1:j+1,:), 1, 1, size(l1data, 3));
l2gain = repmat(gain((i-1)+(width/2):(i+1)+(width/2),j-1:j+1,:), 1, 1, size(l1data, 3));
% Valores medios del entorno a lo largo del tiempo
l1meanPoint = squeeze(mean(mean((l1data - l1dark) .* l1gain, 1), 2));
l2meanPoint = squeeze(mean(mean((l2data - l2dark) .* l2gain, 1), 2));
% Representación de la señal
filterObj = getFilter;
nCoeff = size(filterObj);
filterIndex = nCoeff:size(time) - 1;
timeIndex = 1:size(time) - nCoeff;

% l1
axes(chartHandle1);
datacursormode;
delete(findobj(chartHandle1, 'type', 'line'));
hold on;
plot(time, l1meanPoint, 'blue');
f = filter(filterObj,l1meanPoint);
plot(time(timeIndex), f(filterIndex), 'cyan');
hold off;
% l2
axes(chartHandle2);
datacursormode;
delete(findobj(chartHandle2, 'type', 'line'));
hold on;
plot(time, l2meanPoint, 'red');
f = filter(filterObj,l2meanPoint);
plot(time(timeIndex), f(filterIndex), 'magenta');
hold off;
 
% --- Executes on button press in pushbutton_analisis.
function pushbutton_analisis_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_analisis (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[filename, pathname, ~] = uigetfile('*.exp', 'Selección de fichero de experimento');
if (~isequal(filename,0) && ~isequal(pathname,0));
[width, height, bitpix, dark, gain, timestamps, data] = loadExperimentFile(fullfile(pathname, filename));
end;
analisis (width, height, bitpix, dark, gain, timestamps, data);
