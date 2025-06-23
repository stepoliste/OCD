% function [Vvect, Ivect, Zs] = TransDiodeSamplerNegative(V, I)
% % TransDiodeSampler - Samples I-V data more densely in curved regions
% 
% % Inputs:
% %   V, I - full dense voltage/current vectors from Simscape or similar
% % Outputs:
% %   Vvect, Ivect - sparsely sampled I-V points
% %   Zs - segment impedance between sampled points
% 
% % Ensure column vectors
% V = V(:);
% I = I(:);
% 
% % Sort input by V (important!)
% [V, sortIdx] = sort(V);
% I = I(sortIdx);
% 
% % Compute first and second derivative of I(V)
% dV = diff(V);
% dI = diff(I);
% slope = dI ./ (dV + 1e-12);  % add epsilon to avoid /0
% 
% dslope = diff(slope);
% curvature = [0; abs(dslope); 0];  % pad to match original length
% 
% % Normalize curvature
% curvature = curvature / max(curvature + 1e-12);
% 
% % Select points where curvature is high
% adaptive_threshold = 0.05;  % try 0.01 to 0.1
% adaptive_indices = find(curvature > adaptive_threshold);
% 
% % Always include endpoints
% adaptive_indices = unique([1; adaptive_indices; length(V)]);
% 
% % Add uniformly spaced points to ensure full coverage
% N_uniform = 20;
% uniform_indices = round(linspace(1, length(V), N_uniform))';
% 
% % Combine and sort all selected indices
% all_indices = unique([adaptive_indices; uniform_indices]);
% all_indices = sort(all_indices);
% 
% % Sample V and I
% Vvect = V(all_indices);
% Ivect = I(all_indices);
% 
% % Estimate local impedances between sample points
% Zs = (diff(Vvect) + 1e-12) ./ (diff(Ivect) + 1e-12);
% 
% % Transpose output vectors to row
% Vvect = Vvect';
% Ivect = Ivect';
% 
% end

function [Vvect, Ivect, Zs] = TransDiodeSamplerNegative(V, I)
% TransDiodeSamplerNegative - Samples I-V curve more densely in high-curvature (log-scale) regions

% Ensure column vectors
V = V(:);
I = I(:);

% Sort input by voltage
[V, sortIdx] = sort(V);
I = I(sortIdx);

% Log-scale current for better curvature estimation
logI = log10(abs(I) + eps);  % Add epsilon to avoid log(0)

% First and second derivatives of log(I) vs V
dV = diff(V);
dlogI = diff(logI);
slope = dlogI ./ (dV + eps);

dslope = diff(slope);
curvature = [0; abs(dslope); 0];  % Pad to match original length

% Normalize curvature
curvature = curvature / max(curvature + eps);

% Choose threshold and find high-curvature indices
adaptive_threshold = 0.001;  % Adjust this as needed
adaptive_indices = find(curvature > adaptive_threshold);

% Always include endpoints
adaptive_indices = unique([1; adaptive_indices; length(V)]);

% Add uniform coverage points
N_uniform = 30;
uniform_indices = round(linspace(1, length(V), N_uniform))';

% Merge all indices
all_indices = unique([adaptive_indices; uniform_indices]);
all_indices = sort(all_indices);

% Sampled output
Vvect = V(all_indices);
Ivect = I(all_indices);

% Local impedance estimate
Zs = (diff(Vvect) + eps) ./ (diff(Ivect) + eps);

% Return as row vectors
Vvect = Vvect';
Ivect = Ivect';

end
