length=100;
width=60;
height=40;//35
cornerrad=8;
tubedia=11;
tubeseparation=27.94;//distance between center of tube clusters.
wallthickness=4;
screwx=length/2-1.5*wallthickness;
screwy=width/2-1.5*wallthickness;
screwrad=1;
lpy=52;
lpx=67;
//cube(24);
//88   52           30

$fn=100;

use <MCAD/boxes.scad>
use <MCAD/triangles.scad>
module body(){
mirror([0,0,1]){
difference()
{
translate([0,0,height/2])roundedBox([length,width,height],cornerrad,true);

difference(){
translate([0,0,height/2-7.5])roundedBox([length-wallthickness*2,width-wallthickness*2,height],0,true);

for (a=[1,-1]){
for (b=[1,-1]){
translate([a*length/1.6,b*width/2,height/2-wallthickness])rotate([0,0,45])cube(height,center=true);

}}}

for (c=[-1:1]){
translate([c*tubeseparation,0,0])roundedBox([tubedia*2,tubedia,100],tubedia/2,true);}


//roundedBox([length-wallthickness*1.5,width-wallthickness*1.5,1],0,true);
for (d=[1,-1]){
for (e=[1,-1]){
translate([d*screwx,e*screwy,-1])cylinder(r=screwrad,h=height-5);
}}
cube([length-wallthickness*1.5,width-1.5*wallthickness,wallthickness*2],center=true);
translate([50,9.68,2.49])roundedBox([100,12,13],1,false);
//cube([200,3,9]);
}}}

//translate([0,50,0]){
mirror([0,0,1])
difference(){

union(){
cube([length-wallthickness*1.5-1,width-1.5*wallthickness-1,wallthickness],center=true);
translate([length/2-wallthickness*0.75-1.5,-30/2,-7])cube([4.5,18,9]);



}
translate([-25,-lpy/2,-4])cube([lpx,lpy,4]);
translate([-25,-lpy/2,-3.5])cube([lpx,10,4]);
mirror([0,1,0])translate([-25,-lpy/2,-3.5])cube([lpx,10,4]);
/*translate([0,0,0.2])cylinder(r=10.5,h=10);
translate([27,0,0.2])cylinder(r=10.5,h=10);
translate([-27,0,0.2])cylinder(r=10.5,h=10);
translate([13.5,16,0.2])cylinder(r=10.5,h=10);
translate([13.5,-16,0.2])cylinder(r=10.5,h=10);
translate([-13.5,16,0.2])cylinder(r=10.5,h=10);
translate([-13.5,-16,0.2])cylinder(r=10.5,h=10);*/
for (d=[1,-1]){
for (e=[1,-1]){
translate([d*screwx,e*screwy,-5])cylinder(r=1.8,h=height-5);
translate([d*screwx,e*screwy,-0.7])cylinder(r=3.53,h=height-5);
}}
}