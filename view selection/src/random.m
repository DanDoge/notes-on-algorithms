function [] = random()
    N = 1024;
    a = rand(N, 1) * 2 * pi;
    b = asin(rand(N, 1) * 2 - 1);
    r0 = [cos(a).*cos(b), sin(a).*cos(b), sin(b)];
    v0 = zeros(size(r0));
    G = 1e-2;

    for ii = 1 : 200
        fprintf('%d\n', ii);
        [rn, vn] = countnext(r0, v0, G);
        r0 = rn;
        v0 = vn;
        hold on, scatter3(rn(:, 1), rn(:, 2), rn(:, 3), 'k'), axis equal, axis off, axis image, hold off;
        %figure, patch(rn, 'facecolor', [0.5, 0.5, 0.5]), axis equal, axis off, axis image, hidden on;
    end

    %plot3(rn(:, 1), rn(:, 2), rn(:, 3), '.'); hold on;
    %[xx,yy,zz] = sphere(50);
    %h2 = surf(xx, yy, zz);
    %set(h2, 'edgecolor', 'none', 'facecolor', 'r', 'facealpha', 0.7);
    %axis equal;
    %axis([-1 1 -1 1 -1 1]);
    %hold off;

 end

function [rn vn] = countnext(r, v, G)
    num = size(r, 1);
    dd = zeros(3, num, num);

    for m = 1 : num - 1
        for n = m + 1 : num
            dd(:, m, n) = (r(m, :) - r(n, :))';
            dd(:, n, m) = -dd(:, m, n);
        end
    end

    L = sqrt(sum(dd.^2, 1));
    F = sum(dd./repmat(L.^3, [3 1 1]), 3)';

    Fr = r.*repmat(dot(F, r, 2), [1 3]);
    Fv = F - Fr;

    rn = r + v;
    rn = rn./repmat(sqrt(sum(rn.^2, 2)), [1 3]);
    vn = v + G * Fv;
end
