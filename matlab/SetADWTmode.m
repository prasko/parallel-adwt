% Set default values of the ADWT_mode structure
%
% Description of the structure elements and possible values
%
% ADWT_mode = struct(
%
%%%%%   LIFTING   %%%%%
%   'liftfix':          m1  - number of fixed zeros 
%   'liftvar':          m2  - number of adaptive zeros 
%   'dualfix':          n1  - number of fixed zeros 
%   'dualvar':          n2  - number of adaptive zeros 
%                    'lift' - dual step coefficients equal to lifting step
%   CAUTION: all numbers must be from the set {0, 2, 4, 6} !!!
%
%%%%%   ADAPTATION   %%%%%
%   'adapt':      'lsw'       - Least Squares on Window
%                 'law_lp'    - Least Absolute on Window - linear programming
%                 'law_irwls' - Least Absolute on Window - Iteratively
%                               Re-Weighted Least Squares
%   'N':          window lenght
%
%
% Used by ADWT, IADWT
%
%   D. Sersic, A.Sovic 2011/01/21

global ADWT_mode;

if isempty(ADWT_mode),
   
   N = 31;   % window lenght

   ADWT_mode = struct(...
      'liftfix', 4, ...
      'liftvar', 0, ...
      'dualfix', 2, ...
      'dualvar', 0, ...
      'adapt', 'law_irwls', ...
      'N', N...
      );
   clear N 
end
