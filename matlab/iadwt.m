function [xr] = iadwt(a, d, thp, thu);
% IADWT Single-level inverse discrete 1-D adaptive wavelet transform,
%       with adaptive (time dependent) filter coefficients.
%
%       IADWT performs a single-level 1-D wavelet reconstruction
%       where filter coefficients are adjusted to the signal X 
%       to minimize the square error or the absolute error CD and CA.
% 
%       XR = IADWT(CA,CD,THP,THU) returns the single-level reconstructed 
%       approximation coefficients vector X based on approximation and 
%       detail coefficients vectors CA and CD and filter coefficients in 
%       predict and update lifting steps THP and THU.
%
%       See also: ADWT.
%
%   D. Sersic A. Sovic 2011-01-21


SetADWTmode;


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%        Update             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

nd    = length(d(:,1));   % number of inputs to the adaptation algorithm
th0   = 1;      % initial filter coefficients

d(nd-2:nd)  = [];  d(1:4) = [];
thp(nd-2:nd)  = [];  thp(1:4) = [];
thu(nd-2:nd)  = [];  thu(1:4) = [];
a(nd-3:nd)  = [];  a(1:3) = [];

% S0 - linear update (2 zeros)
xn = [0; d]; x = [d; 0];
x  = (x + xn)/4;
x2 = -x;  

% Delay correction
thp= [th0; thp];
thu= [th0; thu];
d  = [0; d]; a = [a; 0];

% S1 - cubic update (4 zeros)
xn = [0; x; 0]; xnn= [0; 0; x]; x = [x; 0; 0];
x  = (x - 2*xn + xnn)/8;
x4 = x;  

% Delay correction
thp= [th0; thp; th0]; 
thu= [th0; thu; th0]; 
d  = [0;  d; 0]; a = [0; a; 0]; 
x2 = [0; x2; 0];

% S2 - quintic update (6 zeros)
xn = [0; x; 0]; xnn= [0; 0; x]; x = [x; 0; 0];
x  = (x - 2*xn + xnn)*3/16;
x6 = -x;  

% Delay correction
thp= [th0;  thp; th0];
thu= [th0;  thu; th0];
d  = [0;  d; 0]; a  = [0; a;  0];
x2 = [0; x2; 0]; x4 = [0; x4; 0];

% S3 - seventh order update (8 zeros)
xn = [0; x; 0]; xnn= [0; 0; x]; x = [x; 0; 0];
x  = (x - 2*xn + xnn)*5/24;
x8 = x;  

% Delay correction
thp= [th0; thp; th0];
thu= [th0; thu; th0];
d  = [0;  d; 0]; a  = [0; a;  0];
x2 = [0; x2; 0]; x4 = [0; x4; 0]; x6 = [0; x6; 0];

% Fixed part of dual lifting step
switch ADWT_mode.dualfix
   case 2,
      a = a + map2int(x2);  
      switch ADWT_mode.dualvar
         case 0,
            x = zeros(length(a),1);
         case 2,
            x = x4.*thu; 
         case 4,
            x = [x4, x6].*thu;
         case 6,
            x = [x4, x6, x8].*thu;
         otherwise,
            error('Variable part of dual lifting step can have 0, 2, 4 or 6 zeros')
      end
   case 4,
      a = a + map2int(x2 + x4);
      switch ADWT_mode.dualvar
         case 0,
            x = zeros(length(a),1);
         case 2,
            x = x6.*thu;
         case 4,
            x = [x6, x8].*thu;
         otherwise,
            error('Variable part of dual lifting step can have 0, 2 or 4 zeros')
      end
   case 6,
      a = a + map2int(x2 + x4 + x6);
      switch ADWT_mode.dualvar
         case 0,
            x = zeros(length(a),1);
         case 2,
            x = x8.*thu;
         otherwise,
            error('Variable part of dual lifting step can have 0 or 2 zeros')
      end
   otherwise,
      error('Fixed part of dual lifting step can have 2, 4 or 6 zeros')
end

a=a+x;
  

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%        Predict            %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

nd    = length(d(:,1));   % number of inputs to the adaptation algorithm
th0   = 1;      % initial filter coefficients

d(nd-2:nd)  = [];  d(1:4) = [];
thp(nd-2:nd)  = [];  thp(1:4) = [];
thu(nd-2:nd)  = [];  thu(1:4) = [];
a(nd-3:nd)  = [];  a(1:3) = [];


% S0 - linear interpolation (2 zeros)
xn = [0; a]; x = [a; 0];
x  = (x + xn)/2;
x2 = x;  

% Delay correction
a = [0; a]; d = [0; d];
thp= [th0; thp]; 
thu= [th0; thu]; 

% S1 - cubic interpolation (4 zeros)
xnn = [0; 0; x]; xn = [0; x; 0];  x = [x; 0; 0];
x  = (x - 2*xn + xnn)/8;
x4 = -x;

% Delay correction
a  = [0;  a; 0]; d  = [0; d; 0];
x2 = [0; x2; 0];
thp= [th0; thp; th0]; 
thu= [th0; thu; th0]; 

% S2 - quintic interpolation (6 zeros)
xn = [0; x; 0]; xnn = [0; 0; x]; x = [x; 0; 0];
x  = (x - 2*xn + xnn)*3/16;
x6 = x;

% Delay correction
a  = [0;  a; 0]; d  = [0;  d; 0];
x2 = [0; x2; 0]; x4 = [0; x4; 0];
thp= [th0; thp; th0]; 
thu= [th0; thu; th0]; 

% S3 - seventh order interpolation (8 zeros)
xn = [0; x; 0]; xnn = [0; 0; x]; x = [x; 0; 0];
x  = (x - 2*xn + xnn)*5/24;
x8 = -x;

% Delay correction
a  = [0;  a; 0]; d  = [0;  d; 0];
x2 = [0; x2; 0]; x4 = [0; x4; 0]; x6 = [0; x6; 0];
thp= [th0; thp; th0]; 
thu= [th0; thu; th0]; 

% Fixed part of lifting step
switch ADWT_mode.liftfix
   case 2,
      d = d + map2int(x2);
      switch ADWT_mode.liftvar
         case 0,
            x = zeros(length(d),1);
         case 2,
            x = x4.*thp; 
         case 4,
            x = [x4, x6].*thp;
         case 6,
            x = [x4, x6, x8].*thp;
         otherwise,
            error('Variable part of lifting step can have 0, 2, 4 or 6 zeros')
      end
   case 4,
      d = d + map2int(x2 + x4);
      switch ADWT_mode.liftvar
         case 0,
            x = zeros(length(d),1);
         case 2,
            x = x6.*thp;
         case 4,
            x = [x6, x8].*thp;
         otherwise,
            error('Variable part of lifting step can have 0, 2 or 4 zeros')
      end
   case 6,
      d = d + map2int(x2 + x4 + x6);
      switch ADWT_mode.liftvar
         case 0,
            x = zeros(length(d),1);
         case 2,
            x = x8.*thp;
         otherwise,
            error('Variable part of lifting step can have 0 or 2 zeros')
      end
   otherwise,
      error('Fixed part of lifting step can have 2, 4 or 6 zeros')
end
   
d=d+x;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%        Interpolation            %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

nd    = length(d(:,1));   % number of inputs to the adaptation algorithm

d(nd-2:nd)  = [];  d(1:4) = [];
a(nd-2:nd)  = [];  a(1:4) = [];


nd    = length(d(:,1));   % number of inputs to the adaptation algorithm

d(nd-2:nd)  = [];  d(1:4) = [];
a(nd-2:nd)  = [];  a(1:4) = [];

% Polyphase decomposition for decimated filter banks
a = dyadup(a, 0); % a
d = dyadup(d, 1); % d
la = length(a); ld = length(d);
if la > ld, 
    d = [d; 0];
    a = a(1:ld-1);
end  % size correction
if ld > la, 
    a = [a; 0]; 
    d = d(1:ld-1);
end  % size correction

xr=a+d;
      

