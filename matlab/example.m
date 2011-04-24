% example for running functions
%   D. Sersic, A.Sovic 2011/01/21


clear all

testsig;

%% decomposition
tic, [a, d, thp, thu] = adwt(x); toc


figure
plot(thu)
axis tight, hold on
xlabel('n')
title('b')



%% reconstruction
xr = iadwt(a, d, thp, thu);
