//-----------------------------------
// Layers to render
//-----------------------------------

//layer_front(); 
//layer_grid();
//layer_cast();
//layer_support();
//layer_hollow();
//layer_hollow_broken_middle();
//layer_hollow_broken_side();
//layer_back_wall();
//layer_back_v2();
//layer_back_v3(0);

//-----------------------------------
// Components to display
//-----------------------------------

show_big_square_grid=0;
show_small_square_grid=0;
show_two_small_square_grid=0;
show_big_circle_grid=0;
show_small_circle_grid=1;
show_potentiometer=1;
show_microphone=1;
show_fibonacci=0;

//-----------------------------------
// Configuration
//-----------------------------------

// Model size
//model_size=180;
model_size=100;
//model_size=80;

// Grid types (size, step, shift, wall)
grid = [
    [16, 10, 0, 1],
    [8, 8, 0.4, 1]
];

// Circle types (count, inner, outer)
circle = [
    [60, 71.5, 79.5],
    [24, 35.5, 43.5]
];

// Height of the grid (widt of the wood)
wood_width=4;

// Size of an individual LED
led_side=6;

// Shift LED circle 
led_shift = 0;

// Holes radius (half of metric: M3, M4...)
hole_size=2; // M4

// Hole distance to edge
hole_distance=5;

// Minkowski radius
chamfer_size=4;

// Fonts
//font = "Ubuntu:style=Bold";
font = "AG Stencil:style=AG Stencil";

//-----------------------------------
// DO NOT TOUCH BELOW THIS LINE
//-----------------------------------

//-----------------------------------
// Components
//-----------------------------------

module base() {
    
    difference() {
        
        // Shape
        minkowski() {
            square(model_size-2*chamfer_size, true);
            circle(chamfer_size, $fn=50);
        }

        // Holes
        for(i=[0:3]) {
            rotate(90*i) {
                translate([model_size/2-hole_distance,model_size/2-hole_distance,0]) {
                    circle(hole_size, center=true, $fn=50);
                }
            }
        }

    }
}

module fibonacci_block(type, number, x, y) {
    
    size = grid[type][0];
    step = grid[type][1] * (type == 0 ? 2 : 1);
    shift = grid[type][2];
    wall = grid[type][3];

    width = step * number - wall;
    x = x * step + wall/2;
    y = y * step + wall/2;
    translate([x,y]) square(width);

}

module fibonacci_negative(type) {

    size = grid[type][0];
    step = grid[type][1];
    shift = grid[type][2];
    wall = grid[type][3];
    
    translate([shift-size*step/2,-size*step/2]) {
        
        union() {
            fibonacci_block(type, 5, 0, 2);
            fibonacci_block(type, 3, 5, 4);
            fibonacci_block(type, 2, 6, 2);
            fibonacci_block(type, 1, 5, 3);
            fibonacci_block(type, 1, 5, 2);
        }

    }
    
}

module led_matrix_negative(type) {

    size = grid[type][0];
    step = grid[type][1];
    shift = grid[type][2];
    wall = grid[type][3];
    
    translate([shift-size*step/2,-size*step/2])
    for (x=[0:size-1]) {
        for (y=[0:size-1]) {
            translate([x*step+wall/2,y*step+wall/2,0]) {
                square(step-wall);
            }
        }
    }

}

module led_circle_negative(type, led_side, shift) {

    count = circle[type][0];
    inner = circle[type][1];
    outer = circle[type][2];
    radius = (inner + outer)/2;

    for (i=[0:count]) {
        rotate(360 * (i+shift) / count)
            translate([0, radius])
                square([led_side, led_side], true);
    }

}
    
module arc(max, min) {
    union() {
        intersection() {
            difference() {
                circle(max, $fn=50);
                circle(min, $fn=50);
            }
            square(max,center=false);
        }
        translate([0,min]) square([5, 15-min], center=false);
    }
}

module joint() {
    size = hole_size/0.75;
    translate([-size/2-0.25,0])
        union() {
            circle(size/2, $fn=50);
            translate([0,-size/4])
                square([size, size/2]);
        }
}

module potentiometer_negative(mark=0, body=0) {
    if (body==0) circle(4, $fn=50);
    if (mark) {
        translate([7.5,0,0])
            square([1,2.5], true);
        translate([-7.5,0,0])
            square([1,2.5], true);
    }
    if (body==1) circle(16, $fn=50);
}

module mic_negative(body=0) {
    union() {
        circle(5, $fn=50);
        if (body==1) square([12,3], true);
    }
}

module wall_hole() {
    rotate(180)
        minkowski() {
            square([0.1,15]);
            circle(3, $fn=50);
        }
}

module socket_hole() {
    circle(11.4/2, $fn=50);
}

module pcbfootprint_daclock_v2(hole=4) {

    // HOLES
    hole = hole / 2;
    translate([4,4,0]) {
        circle(hole, center=true, $fn=50);
    }
    translate([84,4,0]) {
        circle(hole, center=true, $fn=50);
    }
    translate([4,55,0]) {
        circle(hole, center=true, $fn=50);
    }
    translate([84,55,0]) {
        circle(hole, center=true, $fn=50);
    }
    
    // BUTTONS
    //translate([23,44,0]) {
        //square([54,13]);
    //}
    
    // SPEAKER
    translate([49, 36]) circle(6.5, $fn=50);

}


module pcbfootprint_daclock_v3() {
    
    hole_vector = sqrt(22*22*2);
    
    // PCB holes
    for(i = [45:90:315]) {
        rotate([0,0,i]) {
            translate([0,hole_vector]) {
                circle(1.5, $fn=50);
            }
        }
    }
    
    // speaker
    translate([1,3.5-22]) {
        circle(6.5, $fn=50);
    }
    

}

//-----------------------------------
// Layers
//-----------------------------------

module layer_front() {
    difference() {
        base();
        if (show_potentiometer) translate([0, -15]) potentiometer_negative(0);
        if (show_microphone) translate([0, 15]) mic_negative(0);
    }
}

module layer_grid() {

    difference() {
        
        base();
        
        if (show_big_square_grid) {
            led_matrix_negative(0);
            if (show_fibonacci) fibonacci_negative(0);
        }
        if (show_small_square_grid) {
            led_matrix_negative(1);
            if (show_fibonacci) fibonacci_negative(1);
        }
        if (show_two_small_square_grid) {
            translate([grid[1][0]*grid[1][1]/2-grid[1][2],0]) 
                led_matrix_negative(1);
            translate([-grid[1][0]*grid[1][1]/2-grid[1][2],0]) 
                led_matrix_negative(1);
        }
        if (show_big_circle_grid) 
            led_circle_negative(0, led_side, led_shift);
        if (show_small_circle_grid) 
            led_circle_negative(1, led_side, led_shift);
        
        if (show_potentiometer) translate([0,-15]) potentiometer_negative(1,0);
        if (show_microphone) translate([0,15]) mic_negative(0);
        
    }


}

module layer_cast() {

    difference() {
        
        base();
        
        if (show_big_square_grid) 
            square(grid[0][0]*grid[0][1]+2, true);
        if (show_small_square_grid)
            square(grid[1][0]*grid[1][1]+2, true);
        if (show_two_small_square_grid)
            square([grid[1][0]*grid[1][1]*2+2,grid[1][0]*grid[1][1]+2], true);
        if (show_big_circle_grid) {
            difference() {
                circle(circle[0][2], center=true, $fn=100);
                circle(circle[0][1], center=true, $fn=100);
            }
        }
        if (show_small_circle_grid) {
            difference() {
                circle(circle[1][2], center=true, $fn=100);
                circle(circle[1][1], center=true, $fn=100);
            }
        }
        if (show_potentiometer) translate([0,-15]) potentiometer_negative(1,0);
        if (show_microphone) translate([0,15]) mic_negative(1);
        
    }

}

module layer_support() {

    difference() {
        
        base();
        
        if (show_big_square_grid) {

            translate([
                model_size/2-hole_distance,
                model_size/2-hole_distance
            ]) rotate(180) arc(10,5);
            translate([
                -model_size/2+hole_distance,
                model_size/2-hole_distance
            ]) rotate(180) mirror([1,0,0]) arc(10,5);
        }
        
        if (show_small_square_grid || show_two_small_square_grid) {

            size = grid[1][0];
            step = grid[1][1];
            shift = grid[1][2];
            wall = grid[1][3];
            
            height = size;
            width = show_two_small_square_grid ? height * 2 : height;

            translate([0, step*(height/2-1)])
                square([step*width,step*2], true);
            translate([0, -step*(height/2-1)])
                square([step*width,step*2], true);


            
        }

        if (show_small_circle_grid || show_big_circle_grid) {
            
            inner = circle[show_small_circle_grid ? 1 : 0][1];
            outer = circle[show_small_circle_grid ? 1 : 0][2];
            
            difference() {
                circle(outer, center=true, $fn=100);
                circle(inner, center=true, $fn=100);
                for (i=[0:3]) rotate(90*i) {
                    translate([model_size/2, model_size/2 ]) {
                        square(model_size/1.3, true);
                    }
                }
            }

        }
        
        if (show_potentiometer) translate([0,-15]) potentiometer_negative(1,1);
        if (show_microphone) translate([0,15]) mic_negative(1);

    }

}

module layer_hollow() {
    difference() {
        base();
        difference() {
            square(model_size-2*hole_distance,true);
            for(i=[0:3]) 
                rotate(90*i)
                    translate([
                        model_size/2-hole_distance,
                        model_size/2-hole_distance
                    ])
                        circle(5, $fn=50);
        }
    }
}

module layer_hollow_broken_middle() {
    union() {
        difference() {
            layer_hollow();
            translate([-model_size/2-1,0])
                square([model_size+2,model_size/2+1]);
            translate([model_size/2-hole_distance/2,0])
                rotate(90) joint();
        }
        translate([-model_size/2+hole_distance/2,0])
            rotate(270) joint();
    }
}

module layer_hollow_broken_side() {
    union() {
        difference() {
            layer_hollow();
            translate([0,-model_size/2+2*hole_distance])
                square([model_size/2+2,model_size+1]);
            translate([-model_size/2-1,model_size/2-2*hole_distance])
                square([model_size/2+2,model_size+1]);
            translate([model_size/2-hole_distance/2,-model_size/2+hole_distance*2]) 
                rotate(90) joint();                
        }
        translate([-model_size/2+hole_distance/2,model_size/2-hole_distance*2])
            rotate(270) joint();
    }

}

module layer_back_wall() {
    difference() {
        base();
        translate([-model_size/2 + 15, model_size/2 - 25,0]) wall_hole();
        translate([model_size/2 - 15, model_size/2 - 25,0]) wall_hole();
    }
}

module layer_back_v2() {
    difference() {
        base();
        translate([50,30,0]) wall_hole();
        translate([model_size-50,30,0]) wall_hole();
        translate([(180-51)/2-4,110+(180-80)/2+4,0])
            rotate(-90)
                pcbfootprint_daclock_v3();
    }
}

module layer_back_v3(type) {
    if (type == 1) {
        layer_back_v3_8x8();
    } else {
        layer_back_v3_16x16();
    }
}

module layer_back_v3_16x16() {
    difference() {
        base();
        translate([-model_size/2,-model_size/2]) {
            translate([30,30]) socket_hole();
            translate([30,50]) circle(3.6, $fn=50);
        }
        pcbfootprint_daclock_v3();
    }
}

module layer_back_v3_8x8() {
    difference() {
        base();
        translate([-model_size/2,-model_size/2]) {
            translate([15,15]) socket_hole();
            translate([15,30]) circle(3.6, $fn=50);
        }
        rotate([0,0,180]) pcbfootprint_daclock_v3();
    }
}

// PENDING ***************************************************

/*


module buttons() {
    for (x=[0:3]) {
        translate([0,x*12]) {
            circle(3.6, $fn=50);
            translate([6,-2.5]) 
                text(button_text[x], 6, font,"right","center");
        }
    }
}




module layer_support_circle() {
    difference() {
        base(grid_size, grid_step, chamfer_size, hole_size);
        offset = (grid_size+2)*grid_step/2;
        shift = 360*(0.5-led_angle_shift)/led_count;
        translate([offset,offset]) {
            for (i=[0:3]) {
                rotate(90*i-shift)
                    translate([0, led_radius])
                        square([led_width*2,led_width], true);
            }
        }
        if (led_circle_grid)
            translate([64, 64]) {
                translate([8*0,8*6]) {
                    square([1.75*(8-1),1*4], false);
                }
                translate([8*5,8*0,0]) {
                    square([1.75*(8-1),1*4], false);
                }
            }
    }
}


*/