//-----------------------------------
// CONFIGURATION
//-----------------------------------

// Model size
//model_size=180;
model_size=100;
//model_size=80;

// Number of columns in the square grid
//grid_size=16;
grid_size=8;

// Step between columns
//grid_step=10;
grid_step=8;

// Shitf from the center (due to components on the wall path)
//grid_shift=0;
grid_shift=0.4;

// Width of the grid wall in mm
grid_wall=1;

// Height of the grid (widt of the wood)
grid_height=4;

// Number of LEDs in the circle
//led_count=60;
led_count=24;

// Size of the circle
//led_radius_inner=71.5; // 60 leds
//led_radius_outer=79.5; // 60 leds
led_radius_inner=35.5; // 24 leds
led_radius_outer=43.5; // 24 leds

// Size of an individual LED
led_side=6;

// LED shift in #leds units (0 or 0.5 normally)
led_shift=0;

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
// Components to display
//-----------------------------------

show_big_square_grid=0;
show_small_square_grid=0;
show_circle_grid=1;
show_potentiometer=1;
show_microphone=1;

//-----------------------------------
// Layers to render
//-----------------------------------

// Front face (translucid or dark acrylic, also for the difuser)
//layer_front(); 

// Grid (choose either a 3D model or a 2D model to laser cut)
//layer_grid();

// Layer to hold the PCB matrix layer in place (only for v2)
//layer_cast();
    
// Matrix support layer, with cutouts for the cables
//layer_support();

// Hollow layer
layer_hollow();
//layer_hollow_broken_middle();
//layer_hollow_broken_side();

// Back layer
//layer_back_v1();
//layer_back_v2();

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

module led_matrix_negative(size, step, wall) {

    translate([-size*step/2,-size*step/2])
    for (x=[0:size-1]) {
        for (y=[0:size-1]) {
            translate([x*step+wall/2,y*step+wall/2,0]) {
                square(step-wall);
            }
        }
    }

    }

module led_circle_negative(inner, outer, led_count, led_side, shift) {

    radius = (inner + outer)/2;

    for (i=[0:led_count]) {
        rotate(360 * (i+shift) / led_count)
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
    translate([-hole_size/2,0])
        union() {
            circle(hole_size/2, $fn=50);
            translate([0,-hole_size/4])
                square([hole_size, hole_size/2]);
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
        
        if (show_big_square_grid) 
            led_matrix_negative(16,10,1);
        if (show_small_square_grid) 
            translate([grid_shift,0])
                led_matrix_negative(8,8,1);
        if (show_circle_grid) 
            led_circle_negative(
                led_radius_inner, 
                led_radius_outer, 
                led_count, 
                led_side, 
                led_shift
            );
        if (show_potentiometer) translate([0,-15]) potentiometer_negative(1);
        if (show_microphone) translate([0,15]) mic_negative(0);
        
    }


}

module layer_cast() {

    difference() {
        
        base();
        
        if (show_big_square_grid) square(16*10+2, true);
        if (show_small_square_grid) square(8*8+2, true);
        if (show_circle_grid) {
            difference() {
                circle(led_radius_outer, center=true, $fn=100);
                circle(led_radius_inner, center=true, $fn=100);
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
        
        if (show_small_square_grid) {

            for (i=[0:3]) rotate(90*i) {
                translate([
                        grid_step*(grid_size/2-0.5)+grid_wall/2,
                        grid_step*(grid_size/2-1)
                    ]) square([grid_step*1.4,grid_step*0.5], true);
                translate([
                        grid_step*(grid_size/2-1)+grid_wall/2,
                        grid_step*(grid_size/2-0.5)
                    ]) square([grid_step*0.5,grid_step*1.4], true);
            }


            
        }

        if (show_circle_grid) {
            
            difference() {
                circle(led_radius_outer, center=true, $fn=100);
                circle(led_radius_inner, center=true, $fn=100);
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



// PENDING ***************************************************

/*


module wall_hole() {
    minkowski() {
        square([0.1,15], center=true);
        circle(3, $fn=50);
    }
}

module socket_hole() {
    circle(11.4/2, $fn=50);
}

module buttons() {
    for (x=[0:3]) {
        translate([0,x*12]) {
            circle(3.6, $fn=50);
            translate([6,-2.5]) 
                text(button_text[x], 6, font,"right","center");
        }
    }
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

module layer_back_v1() {
    difference() {
        base(grid_size, grid_step, chamfer_size, hole_size);
        translate([50,30,0]) wall_hole();
        translate([(grid_size+2)*grid_step-50,30,0]) wall_hole();
        translate([(180-51)/2-4,110+(180-80)/2+4,0])
            rotate(-90)
                pcbfootprint();
    }
}

module layer_back_v2() {
    if (grid_size == 8) {
        layer_back_v2_8x8();
    } else {
        layer_back_v2_16x16();
    }
}

module layer_back_v2_16x16() {
    difference() {
        base(grid_size, grid_step, chamfer_size, hole_size);
        translate([30,30]) socket_hole();
        translate([30,50]) circle(3.6, $fn=50);
        translate([90,90])
            pcbfootprint_v2();
    }
}

module layer_back_v2_8x8() {
    difference() {
        base(grid_size, grid_step, chamfer_size, hole_size);
        translate([15,15]) socket_hole();
        translate([15,30]) circle(3.6, $fn=50);
        translate([48,48])
            rotate([0,0,180]) pcbfootprint_v2();
    }
}
*/