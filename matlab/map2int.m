function y = map2int(x)
% Maps input data to integers, depending on global variable
% RLS_map_int_to_int
global RLS_map_int_to_int

if ~isempty(RLS_map_int_to_int),
   if RLS_map_int_to_int ~= 0,
      y = round(x);
   else
      y = x;
   end
else
   y = x;
end
