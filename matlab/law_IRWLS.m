function [thm, yhat, yd] = law_IRWLS(z, nn, N, th0, norm);
%LAW_IRWLS    Computes windowed LS-estimates of ARX-models.
%   [THM,YHAT,YD] = LAW_IRWLS(Z,NN,N,th0)
%
%   THM: returned as the estimated parameters of the ARX model
%   A(q) y(t) = B(q) u(t-nk) + e(t), calculated on
%   the window of N input samples.
%   YHAT: The predicted values of the outputs. Last YHAT row corresponds to
%   time of the last Z row.
%   YD: Prediction error (YD = YHAT-Y). Last YD row corresponds to
%   time of the last Z row.
%
%   Z : the output-input data Z=[y u], with y and u as column vectors.
%   For multivariable systems Z=[y1 y2 .. yp u1 u2 .. um]. For time series
%   Z=y only.
%
%   NN: NN = [na nb nk], the orders and delays of the above model.
%   N: length of input samples taken into the account. 
%   th0:  Initial value of estimated parameters. 
%
%   See also LS, ARX, LAW_LP, LSW
%
%   D. Sersic and A. Sovic 2011-01-21


[nz,ns]=size(z);  
[nr,nc]=size(nn);  

if ns == 1, if length(nn) ~= 1,
   error('For a time series nn should be a scalar nn = na!')
end,end

if nr>1, error('Multiple outputs not supported!'), return, end
if 2*ns-1 ~= length(nn)
   disp('Incorrect number of orders specified!')
   disp('For an AR-model nn=na'),disp('For an LS-model, nn=[na nb nk]')
   error, return,
end

na = nn(1);  

if ns > 1, % Input and output
   nb = nn(2:ns);          % Number of b coefficients
   nk = nn(ns+1:2*ns-1);   % Number of delays
else       % Input only
   nb = 0; 
   nk = 1; 
end
if (na < 0) | (any(nb) < 0)
   error('All orders must be non-negative.'), return,
end


nab    = na + sum(nb); % Total number of coefficients (a and b)
if nab == 0, return, end

nak = abs(min(0, min(nk))); % Largest negative delay
nbm  = nb + nk + nak;       % Data length of each input (incl. delays)
ncbm = na + nak + cumsum([0 nbm]); % New output/input data indexes
dm   = na + nak + sum(nbm); % Length of phi vector (including delays)
% Indexes for shifting output and input components of phi
io = [1:na+nak-1]; ii = [na+nak+1: dm-1]; 
                                  
% Indexes of active components of phi (without delays)
i = [nak+1:nak+na];      % Output indexes
for ku = 1:ns-1,     % All inputs
   i = [i ncbm(ku)+nk(ku)+nak+1:ncbm(ku+1)]; % Input indexes
end

% Allocate space  %???????????????''''''
yhat = zeros(nz, 1);   % errors
yd   = zeros(nz, 1);   % outputs
thm  = zeros(nz, nab); % parameters of filters

if nargin < 4, th = zeros(nab,1); 
elseif isempty(th0), th = zeros(nab,1);
else th = th0(:); end

if nargin < 5, norm = 1; 
elseif isempty(norm), norm = 1;
end

% initial values  ?????????????????
phi   = zeros(dm,1);    % inputs
wphiw = zeros(N, nab);  % weighted inputs
phiw  = zeros(N, nab);  % inputs on the window
yw    = zeros(N,1);     % outputs on the window


% prosirivanje da bi kasnije mogli staviti u sredinu ?????????
z=[z; zeros(ceil(N/2),ns)];
nz=size(z,1);

% Construct output vector
y = zeros(nz,1);
y(1:nak) = -phi(nak:-1:1);
y(nak+1:nz) = z(1:nz-nak, 1);

   
for kcou=1:nz
       
     % Load input data 
     if any(ncbm>0),
         phi(ii+1) = phi(ii); % Shift input vector
         phi(ncbm(1:ns-1)+1) = z(kcou,2:ns)'; 
     end

     % remembering the parameters of the filter
     thm(kcou,:)=th';  

     % Extract sliding window
     phiw(1 ,:) = [];
     phiw(N,:) = phi(i)';
       
     yw(1,:) = [];
     yw(N,:) = y(kcou);

     % initial values for IRWLS
     all_th=[];
     weight = ones(N,1);
              
     stop=0;
     iter=0;
       
     % Iteratively Re-Weighted Least Squares
     while ((stop | (iter<2)) & (iter<200))
           iter=iter+1;
                      
           % slack variable
           for k=1:N 
               wphiw(k,:) = weight(k) * phiw(k,:); 
           end

        
           % Construct R & F matrices
           R = phiw' * wphiw;       
           F = wphiw' * yw;          

           % Compute the estimate
           th = pinv(R) * F;
           all_th(iter,:)=th';
           
           % Compute new weight vector
           err = phiw * th - yw;  % Error       
           
           estd = std(err);          
           err = err / (abs(estd) + eps);
       
           weight = ((abs(err)).^(norm) + eps) ./ ((err .* err) + eps);
          
                   
           %stop criterion 
           if iter>=2
                tmp=(abs(all_th(iter-1,:)'-th)>0.0001);
                if find(tmp==1), stop=1;
                else stop=0; end
           end
     end

   th(find(th<0))=0;  % for negative parameter filter is low-pass ????????????
   if any(isnan(th)), th=thm(kcou,:); end

  % Load output data 
  if na>0, 
     phi(io+1) = phi(io); % Shift output vector 
     phi(1) = -z(kcou,1); 
  end
end

     
% za stavljanje u sredinu ??????????????????
for kcou=1:size(z,1)-floor(N/2)
     yhat(kcou) = thm(floor((N)/2)+kcou)*z(kcou,2);
     yhat(kcou) = map2int(yhat(kcou));
     yd(kcou)   = yhat(kcou) - z(kcou,1);
end

thm = thm(floor(N/2)+1:end,:);
yhat(end)=[];
yd(end)=[];
