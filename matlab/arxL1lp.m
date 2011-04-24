function p = arxL1lp(z,orders,p,wphi)
%ARXL1lp    Computes least absolute estimates of ARX-models (minimum L1
%   error) using linear programming
%
%   P = ARXL1LP(DATA,ORDERS)
%
%   P: estimated parameters of the ARX model
%   A(q) y(t) = B(q) u(t-nk) + e(t)
%     or
%   A(q) y(t) = B1(q) u1(t-nk) + B2(q) u2(t-nk) + ... + Bm(q) um(t-nk) + e(t)
%
%   DATA: The estimation data is formed as [y u] for single input
%   systems, or [y u1 u2 ... um] for multi-input systems.
%   
%   ORDERS = [na nb nk], the orders and delays of the ARX model.
%   For multi-input systems, nb and nk are specified for each input.
%
%   ARXL1LP(DATA,ORDERS,P0) uses initial value P0 of the model
%   parameters.
%
%   In ARXL1LP(DATA,ORDERS,P0,W,WPHI), WPHI determines which part of shifted data matrix 
%   is kept in calculation: 'f','c' and 'l' correspond to first, central and last part, 
%   'a' corresponds to the whole matrix. Default is 'l'.
%
% See also ARXL2, ARX.


%   D. Sersic and A. Sovic
%   Copyright 2010 FER-ZESOI
%   $Revision: 2.00.0.1 $  $Date: 2011/01/21 12:00:00 $

nx=size(z,2)-1;  % number of inputs

na=orders(1,1);          % number of output parameters
nb=orders(1,2:1+nx);     % number of parameters of each input
nk=orders(1,2+nx:end);   % delay of each input

y = z(:,1);
x = z(:,2:nx+1);

np=na+sum(nb); % total number of estimated parameters


if (nx==0) && (length(orders)~=1)
    error('identL1:arxL1:BadInput', 'For a time series ORDERS should be a scalar: ORDERS = na.')
end

if (2*nx+1) ~= length(orders) 
    error('identL1:arxL1:BadInput', 'Incorrect number of orders specified. ORDERS = [na nb nk].')
end

if any(na<0) || any(nb<0)
    error('identL1:arxL1:BadInput', 'Orders must be non-negative values.')
end


wn=size(y,1);
if ~isempty(nk)
    nkmax=min(nk);
else
    nkmax=0;
end
if nkmax>0, nkmax=0; end
phi=zeros(wn+max([nk+nb na]-1),np, class(x));
for k=1:na
    phi(k+1-nkmax:wn+k-nkmax,k)=y(1:wn);
end
for kx=1:nx
    for k=(na+1)+sum(nb(1:kx-1)):na+sum(nb(1:kx-1))+nb(kx)
        phi(k-na+nk(1,kx)-sum(nb(1:kx-1))-nkmax:wn+(k-na+nk(1,kx)-sum(nb(1:kx-1))-nkmax)-1,k) = x(1:wn,kx);
    end
end
y=[y; zeros(size(phi,1)-size(y,1),1)];

if nargin <= 3 || isempty(wphi)
    wphi='l';
end

% Different data windows
if strcmp(wphi,'f') || strcmp(wphi,'F')     % first
    phi=phi(1:wn,:);
    y=y(1:wn,:);
elseif strcmp(wphi,'c') || strcmp(wphi,'C') % central
    phi=phi(end-wn+1:wn,:);
    y=y(end-wn+1:wn,:);
elseif strcmp(wphi,'l') || strcmp(wphi,'L') % last (default)
    phi=phi(end-wn+1:end,:);
    y=y(end-wn+1:end,:);
end % otherwise: all data
    
wn=size(y,1); 


% Initialize the estimated parameters
if nargin < 3 || isempty(p) 
    p=zeros(1,np, class(x));
end


p = linprog([zeros(1,np),ones(1,2*wn)]',[],[],[phi, speye(wn), -speye(wn)],y,[-inf(1,np),zeros(1,2*wn)],[],p);
p = p(1:np);
p = p(:)';



p(1:na)=-p(1:na); % Reverse signs of output parameters

