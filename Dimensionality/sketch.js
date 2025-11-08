// Copyright (c) 2025, Jake Morgan, termite
// Inside this code we are utilizing p5js' WEBGL tools to display the vertex point cloud.
// This code is free to use for any purpose.
/*
                 _ _                        _       _   _             
                | (_)                      | |     | | (_)            
  __ _ _   _  __| |_  ___    ___  ___ _   _| |_ __ | |_ _ _ __   __ _ 
 / _` | | | |/ _` | |/ _ \  / __|/ __| | | | | '_ \| __| | '_ \ / _` |
| (_| | |_| | (_| | | (_) | \__ \ (__| |_| | | |_) | |_| | | | | (_| |
 \__,_|\__,_|\__,_|_|\___/  |___/\___|\__,_|_| .__/ \__|_|_| |_|\__, |
                                             | |                 __/ |
                                             |_|                |___/ 
*/
//
// info:
// in this code I use the p5js/webgl library to create a pointcloud
// that represents our shape and our sculpted audio.
// At the moment... there is no way to edit the shape in realtime.
// that's why there's an upload option.
// In the future IE: my BFA Thesis this will be possible.
// imagine blender for audio.
// thankfully a very short file since there's no real GLSL code here.
// WEBGL/OpenGL is just allowing our display to exist :)

let sketch_dimensionality = function(p){
  let vertices = [];
  let rotX = 0, rotY = 0, rotZ = 0;
  
  p.setup = function(){
    p.createCanvas(600, 600, p.WEBGL);
    p.angleMode(p.DEGREES);
    console.log('Canvas created');
  };
  
  p.draw = function(){
    p.background(252, 252, 255);
    
    p.ambientLight(100);
    p.directionalLight(255, 255, 255, 0, 0, -1);
    p.pointLight(255, 255, 255, 100, -100, 100);
    
    p.push();
    
    rotX += 0.3;
    rotY += 0.02;
    rotZ += 0.06;
    p.rotateX(rotX);
    p.rotateY(rotY);
    p.rotateZ(180 + rotZ); 
    p.normalMaterial();
    p.noStroke();
    
    if (vertices.length > 0) {
      p.scale(200);
      const factor = 2.875; 
      p.strokeWeight(2);
      p.stroke(255, 100, 100);
      p.beginShape(p.POINTS);
      for (let v of vertices) {
        p.vertex(v.x * factor, v.y * factor, v.z * factor);
      }
      p.endShape();
      
    } else {
      // cube when no obj loaded.
      p.scale(70);
      p.box(1);
    } 
    p.pop();
  };
 
  // load from external
  p.updateVertices = function(newVertices) {

    let minX = Infinity, maxX = -Infinity;
    let minY = Infinity, maxY = -Infinity;
    let minZ = Infinity, maxZ = -Infinity;
    
    for (let v of newVertices) {
      minX = Math.min(minX, v.x);
      maxX = Math.max(maxX, v.x);
      minY = Math.min(minY, v.y);
      maxY = Math.max(maxY, v.y);
      minZ = Math.min(minZ, v.z);
      maxZ = Math.max(maxZ, v.z);
    }
    
    const scaleX = maxX - minX || 1;
    const scaleY = maxY - minY || 1;
    const scaleZ = maxZ - minZ || 1;
    const maxScale = Math.max(scaleX, scaleY, scaleZ) * 2;
    
    const centerX = (minX + maxX) / 2;
    const centerY = (minY + maxY) / 2;
    const centerZ = (minZ + maxZ) / 2;
    
    vertices = newVertices.map(v => ({
      x: (v.x - centerX) / maxScale,
      y: (v.y - centerY) / maxScale,
      z: (v.z - centerZ) / maxScale
    }));
    
    console.log(`Updated with ${vertices.length} vertices`);
  };
};

let dimensionality_p5 = new p5(sketch_dimensionality, 'canvas-container-dimensionality');

document.addEventListener('DOMContentLoaded', () => {
  const originalObjHandler = document.getElementById('obj-input').onchange;
  
  document.getElementById('obj-input').addEventListener('change', async (e) => {
    const file = e.target.files[0];
    if (file) {
      try {
        const text = await file.text();
        
        const vertices = [];
        const lines = text.split('\n');
        
        for (const line of lines) {
          const parts = line.trim().split(/\s+/);
          if (parts[0] === 'v') {
            vertices.push({
              x: parseFloat(parts[1]),
              y: parseFloat(parts[2]), 
              z: parseFloat(parts[3])
            });
          }
        }
        
        if (dimensionality_p5 && vertices.length > 0) {
          dimensionality_p5.updateVertices(vertices);
        }
        
        console.log(`Parsed ${vertices.length} vertices for display`);
        
      } catch (error) {
        console.error('Error parsing OBJ:', error);
      }
    }
  });
});
