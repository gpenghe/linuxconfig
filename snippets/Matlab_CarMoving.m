function CarMoving

%% Configurations
drive_n_x = 2;
drive_n_y = 10;
drive_space_x = 70;
drive_space_y = 10;
drive_width = 100;
drive_height = 50;
car_width = 40;
car_height = 200;

drive_offset_x = 50;
drive_offset_y = 300;
car_offset_y = 50;

%% Draw Drives 
newplot; % Initialize gca

for j = 1 : drive_n_y
    for i = 1 : drive_n_x
        x_start = - (drive_space_x /2 + drive_width);
        y_start = drive_offset_y;
        sx = drive_width + drive_space_x;
        sy = drive_height + drive_space_y;
        hDriveRect(j, i) = rectangle( ...
            'Position', [x_start + (i-1)*sx, y_start + (j-1)*sy, ...
                drive_width, drive_height] ...
            , 'EdgeColor', 'b' ...
            , 'LineWidth', 2);
    end
end

%% Draw the Car
hCarRect = rectangle( ...
            'Position', [- car_width/2, 0  ...
                car_width, car_height] ...
            , 'EdgeColor', 'g' ...
            , 'LineWidth', 3);

%% Fire up the animation
pos = [0:1:100];

anymate(@plotone, pos, 'Fps', 25);

% Define a helper function to do the plotting
    function h = plotone(y)
        ylim([0, 1400]);
        pos = get(hCarRect,'Position');
        pos(2) = y*10;
        set(hCarRect,'Position', pos);
        h = hCarRect;
    end
end
