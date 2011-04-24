function [a, d, thp, thu] = adwt(x, thp, thu);
% ADWT Single-level discrete 1-D adaptive wavelet transform,
%       with adaptive (time dependent) filter coefficients.
%
%       ADWT performs a single-level 1-D wavelet decomposition
%       where filter coefficients are adjusted to the signal X 
%       to minimize the square error or absolute error CD and CA.
% 
%       [CA,CD,THP,THU] = ADWT(X) computes the approximation
%       coefficients vector CA and detail coefficients vector CD,
%       obtained by adaptive wavelet decomposition of the vector X.
%
%       THP and THU are filter coefficients in predict and update 
%       lifting steps. They corespond to the coefficients of H and L 
%       filters, respectivelly.
%
%       [CA,CD] = ADWT(X, THP, THU) uses forced filter coefficients 
%       THP and THU.
%
%       See also: IADWT.
%
%   D. Sersic, A.Sovic 2011/01/21

SetADWTmode;


% Polyphase decomposition for decimated filter banks
a = dyaddown(x, 1); % odd
d = dyaddown(x, 0); % even

la = length(a); ld = length(d);
if la > ld, d = [d; 0]; end  % size correction


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%        Predict            %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% S0 - linear interpolation (2 zeross)
xn = [0; a]; x = [a; 0];
x  = (x + xn)/2;
x2 = x;  

% Delay correction 
a = [0; a]; d = [0; d];

% S1 - cubic interpolation (4 zeros)
xnn = [0; 0; x]; xn = [0; x; 0];  x = [x; 0; 0];
x  = (x - 2*xn + xnn)/8;
x4 = -x;

% Delay correction 
a  = [0;  a; 0]; d  = [0; d; 0];
x2 = [0; x2; 0];

% S2 - quintic interpolation (6 zeros)
xn = [0; x; 0]; xnn = [0; 0; x]; x = [x; 0; 0];
x  = (x - 2*xn + xnn)*3/16;
x6 = x;

% Delay correction
a  = [0;  a; 0]; d  = [0;  d; 0];
x2 = [0; x2; 0]; x4 = [0; x4; 0];

% S3 - seventh order interpolation (8 zeros)
xn = [0; x; 0]; xnn = [0; 0; x]; x = [x; 0; 0];
x  = (x - 2*xn + xnn)*5/24;
x8 = -x;

% Delay correction
a  = [0;  a; 0]; d  = [0;  d; 0];
x2 = [0; x2; 0]; x4 = [0; x4; 0]; x6 = [0; x6; 0];

% Fixed part of lifting step
switch ADWT_mode.liftfix
   case 0,
      switch ADWT_mode.liftvar
         case 0,
            error('You need to have any varialbe or fixed part.')  %?????????
         case 2,
            x = x2; 
         case 4,
            x = [x2, x4];
         case 6,
            x = [x2, x4, x6];
         case 8,
            x = [x2, x4, x6, x8];
         otherwise,
            error('Variable part of lifting step can have 0, 2, 4, 6 or 8 zeros')
      end

   case 2,
      d = d - map2int(x2);
      switch ADWT_mode.liftvar
         case 0,
         case 2,
            x = x4; 
         case 4,
            x = [x4, x6];
         case 6,
            x = [x4, x6, x8];
         otherwise,
            error('Variable part of lifting step can have 0, 2, 4 or 6 zeros')
      end
   case 4,
      d = d - map2int(x2 + x4);
      switch ADWT_mode.liftvar
         case 0,
         case 2,
            x = x6;
         case 4,
            x = [x6, x8];
         otherwise,
            error('Variable part of lifting step can have 0, 2 or 4 zeros')
      end
   case 6,
      d = d - map2int(x2 + x4 + x6);
      switch ADWT_mode.liftvar
         case 0,
         case 2,
            x = x8;
         otherwise,
            error('Variable part of lifting step can have 0 or 2 zeros')
      end
   otherwise,
      error('Fixed part of lifting step can have 2, 4 or 6 zeros')
end




   
nx    = length(x(1,:));   % number of inputs to the adaptation algorithm
th0   = ones(1, nx);      % initial filter coefficients
order = [0 ones(1, nx) zeros(1, nx)];  % adaptive model order

xr=x;  % for saving original inputs  %?????????????????
dr=d;  % for saving original outputs  %????????????????????

if nargin>1 && ~isempty(thp)
    th0(1,:)=thp;
end

if 0 ~= ADWT_mode.liftvar
   switch ADWT_mode.adapt
     case 'lsw',
        [thp, dhat, d] = lsw([d x], order, ADWT_mode.N, th0);
              
     case 'law_lp',
        [thp, dhat, d] = law_lp([d x], order, ADWT_mode.N, th0);
       
     case 'law_irwls',
        [thp, dhat, d] = law_IRWLS([d x], order, ADWT_mode.N, th0,1);

     otherwise,
       error('Adaptation algorithm can be rsw, law_lp or law_irwls.')
   end

   if size(thp,1)~=size(xr,1), thp(end,:)=[]; end
   d=dr-sum(xr.*thp,2);

else
   thp = [];
end

      
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%        Update             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


% S0 - linear update (2 zeros)
xn = [0; d]; x = [d; 0];
x  = (x + xn)/4;
x2 = -x;  

% Delay correction
thp= [th0; thp];
d  = [0; d]; a = [a; 0];  

% S1 - cubic update (4 zeros)
xn = [0; x; 0]; xnn= [0; 0; x]; x = [x; 0; 0];
x  = (x - 2*xn + xnn)/8;
x4 = x;  

% Delay correction
thp= [th0; thp; th0]; 
d  = [0;  d; 0]; a = [0; a; 0]; 
x2 = [0; x2; 0];

% S2 - quintic update (6 zeros)
xn = [0; x; 0]; xnn= [0; 0; x]; x = [x; 0; 0];
x  = (x - 2*xn + xnn)*3/16;
x6 = -x;  

% Delay correction
thp= [th0;  thp; th0];
d  = [0;  d; 0]; a  = [0; a;  0];
x2 = [0; x2; 0]; x4 = [0; x4; 0];

% S3 - seventh order update (8 zeros)
xn = [0; x; 0]; xnn= [0; 0; x]; x = [x; 0; 0];
x  = (x - 2*xn + xnn)*5/24;
x8 = x;  

% Delay correction
thp= [th0; thp; th0];
d  = [0;  d; 0]; a  = [0; a;  0];
x2 = [0; x2; 0]; x4 = [0; x4; 0]; x6 = [0; x6; 0];

% Fixed part of dual lifting step
switch ADWT_mode.dualfix
   case 0,
      switch ADWT_mode.dualvar
         case 0,
            error('You need to have any varialbe or fixed part.')  %?????????
         case 2,
            x = x2;
         case 4,
            x = [x2, x4]; 
         case 6,
            x = [x2, x4, x6];
         case 8,
            x = [x2, x4, x6, x8];
        otherwise,
            error('Variable part of dual lifting step can have 0, 2, 4 or 6 zeros')
      end
   case 2,
      a = a - map2int(x2);
      switch ADWT_mode.dualvar
         case 0,
         case 2,
            x = x4; 
         case 4,
            x = [x4, x6];
         case 6,
            x = [x4, x6, x8];
         otherwise,
            error('Variable part of dual lifting step can have 0, 2, 4 or 6 zeros')
      end
   case 4,
      a = a - map2int(x2 + x4);
      switch ADWT_mode.dualvar
         case 0,
         case 2,
            x = x6;
         case 4,
            x = [x6, x8];
         otherwise,
            error('Variable part of dual lifting step can have 0, 2 or 4 zeros')
      end
   case 6,
      a = a - map2int(x2 + x4 + x6);
      switch ADWT_mode.dualvar
         case 0,
         case 2,
            x = x8;
         otherwise,
            error('Variable part of dual lifting step can have 0 or 2 zeros')
      end
   otherwise,
      error('Fixed part of dual lifting step can have 2, 4 or 6 zeros')
end 

   
nx    = length(x(1,:));    % number of inputs to the adaptation algorithm
th0   = ones(1, nx);       % initial filter coefficients
order = [0 ones(1, nx) zeros(1, nx)];  % adaptive model order
thu = thp;

xr=x;  % for saving original inputs  %?????????????????
ar=a;  % for saving original outputs  %????????????????????

if nargin>2 && ~isempty(thu)
    th0(1,:)=thu;
end

if 0 ~= ADWT_mode.dualvar
   switch ADWT_mode.adapt
     case 'lsw',
        [thu, ahat, a] = lsw([a x], order, ADWT_mode.N, th0);
              
     case 'law_lp',
        [thu, ahat, a] = law_lp([a x], order, ADWT_mode.N, th0);
       
     case 'law_irwls',
        [thu, ahat, a] = law_IRWLS([a x], order, ADWT_mode.N, th0,1);

     otherwise,
       error('Adaptation algorithm can be rsw, law_lp or law_irwls.')
   end

   if size(thu,1)~=size(xr,1), thu(end,:)=[]; end
   a=ar-sum(xr.*thu,2);

end

% d=d(8:end-7,:);
% a=a(8:end-7,:);
% thp=thp(8:end-7,:);
% thu=thu(8:end-7,:);
