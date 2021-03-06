function Plot_Compare_N2()
% This function creates Figure 5 of
%
% Characterization of K-Complexes and Slow Wave Activity in a Neural Mass Model
% A Weigenand, M Schellenberger Costa, H-VV Ngo, JC Claussen, T Martinetz
% PLoS Computational Biology. 2014;10:e1003923
% Load the data

% Path to utility functions
if(isempty(strfind(path, [pwd, '/Tools'])))
    addpath([pwd, '/Tools']);
end

% Load the data
load('Data/EEG_Data_N2.mat')
load('Data/Simulation_N2.mat')

% Zscore the simulation data
Ve_N2 = zscore(Ve_N2); %#ok<NODEF>

% Set the time axis
timeaxis = linspace(0, 30, 3000);

% Create figure handle
figure(1)
clf,shg

% Plot the data
plot(timeaxis,Data_N2+4,'-k','LineWidth',0.5)
hold on
plot(timeaxis,Ve_N2-4,'-k','LineWidth',0.5);

% Remove the axes 
axis off

% Add the xlabel and the figure caption
text(-1.5,7,'(a)')
text(-1.5,-2,'(b)')

% Add a 5s scale
plot([2,7], [-9,-9],'-k','LineWidth',2)
text(4.5,-10,'5s')
hold off
end
