%% signal example
%   D. Sersic, A.Sovic 2011/01/21

t = [0:99]';

x1 = 2*cos(pi/6*t)+2/400*t;
x3 = 1.5*cos(3*pi/16*t)+3;
x2 = cos(pi/3*t)+3-1/400*t;
x4 = 0.5*cos(10*pi/9*t)+1.5;
x5 = 2.5*cos(pi/8*t)-0.56+3/400*t;


x=[x1; x2; x3; x4; x5];
x(500)=6;
x(300)=-2;

% s = 0.4 * randn(size(x,1),1);
% s = sign(s) .* s .* s / sqrt(3);

clear x1 x2 x3 x4 x5 x6  t
