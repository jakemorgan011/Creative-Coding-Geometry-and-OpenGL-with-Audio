/*
  ==============================================================================

    OpenGl.h
    Created: 2 Jul 2025 4:00:23pm
    Author:  Jake Morgan

  ==============================================================================
*/

#pragma once
//#include <JuceHeader.h>
#include <juce_opengl/juce_opengl.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "WavefrontObjParser.h"
#include "BinaryData.h"
// ALRIGHT
// MAKING A PLUGIN AGAIN
// HERES THE PLAN:
// 1. GET THE HORSE OBJ ON SCREEN. 
// 2. MAKE IT SO YOU CAN GRAB IT WITH MOUSE AND ROTATE IT.
// 3. MAKE REVERB
// 4. MAKE ROTATION AFFECT REVERB
// 5. ADD KEYPRESS TO CHANGE HORSE DISTANCE AND WHAT NOT
// 6. MAKE THAT REFLECT IN REVERB -- you are here.
// 7. MAKE SURE THAT ALL SAVES TO XML AND CAN BE LOADED.
// 8. TRIPLE CHECK FOR MEMORY LEAKS.
// 9. GO BACK AND FIX THE XML
// 10. PRAY TO GOD IT WORKS.
// 11. CALL JAKE CHEN AND ASK HIM FOR HELP WITH XML

// BEGIN
// USING CLASSES FROM OPENGL DEMO FROM JUCER
// GOING TO COPY THE "TOON SHADER" FRAGMENT SHADER FROM DOCS

// forward declaration
//class TheHorsePluginAudioProcessor;

using namespace juce;
struct OpenGLUtils
{
    //==============================================================================
    /** Vertex data to be passed to the shaders.
        For the purposes of this demo, each vertex will have a 3D position, a colour and a
        2D texture co-ordinate. Of course you can ignore these or manipulate them in the
        shader programs but are some useful defaults to work from.
     */
    struct Vertex
    {
        float position[3];
        float normal[3];
        float colour[4];
        float texCoord[2];
    };

    //==============================================================================
    // This class just manages the attributes that the demo shaders use.
    struct Attributes
    {
        explicit Attributes (OpenGLShaderProgram& shader)
        {
            position      .reset (createAttribute (shader, "position"));
            normal        .reset (createAttribute (shader, "normal"));
            sourceColour  .reset (createAttribute (shader, "sourceColour"));
            textureCoordIn.reset (createAttribute (shader, "textureCoordIn"));
        }

        void enable()
        {
            using namespace ::juce::gl;

            if (position.get() != nullptr)
            {
                glVertexAttribPointer (position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), nullptr);
                glEnableVertexAttribArray (position->attributeID);
            }

            if (normal.get() != nullptr)
            {
                glVertexAttribPointer (normal->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 3));
                glEnableVertexAttribArray (normal->attributeID);
            }

            if (sourceColour.get() != nullptr)
            {
                glVertexAttribPointer (sourceColour->attributeID, 4, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 6));
                glEnableVertexAttribArray (sourceColour->attributeID);
            }

            if (textureCoordIn.get() != nullptr)
            {
                glVertexAttribPointer (textureCoordIn->attributeID, 2, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 10));
                glEnableVertexAttribArray (textureCoordIn->attributeID);
            }
        }

        void disable()
        {
            using namespace ::juce::gl;

            if (position != nullptr)        glDisableVertexAttribArray (position->attributeID);
            if (normal != nullptr)          glDisableVertexAttribArray (normal->attributeID);
            if (sourceColour != nullptr)    glDisableVertexAttribArray (sourceColour->attributeID);
            if (textureCoordIn != nullptr)  glDisableVertexAttribArray (textureCoordIn->attributeID);
        }

        std::unique_ptr<OpenGLShaderProgram::Attribute> position, normal, sourceColour, textureCoordIn;

    private:
        static OpenGLShaderProgram::Attribute* createAttribute (OpenGLShaderProgram& shader,
                                                                const char* attributeName)
        {
            using namespace ::juce::gl;

            if (glGetAttribLocation (shader.getProgramID(), attributeName) < 0)
                return nullptr;

            return new OpenGLShaderProgram::Attribute (shader, attributeName);
        }
    };

    //==============================================================================
    // This class just manages the uniform values that the demo shaders use.
    struct Uniforms
    {
        explicit Uniforms (OpenGLShaderProgram& shader)
        {
            projectionMatrix.reset (createUniform (shader, "projectionMatrix"));
            viewMatrix      .reset (createUniform (shader, "viewMatrix"));
            modelMatrix     .reset (createUniform (shader, "modelMatrix"));
            texture         .reset (createUniform (shader, "demoTexture"));
            lightPosition   .reset (createUniform (shader, "lightPosition"));
            bouncingNumber  .reset (createUniform (shader, "bouncingNumber"));
        }

        std::unique_ptr<OpenGLShaderProgram::Uniform> projectionMatrix, viewMatrix, modelMatrix, texture, lightPosition, bouncingNumber;

    private:
        static OpenGLShaderProgram::Uniform* createUniform (OpenGLShaderProgram& shader,
                                                            const char* uniformName)
        {
            using namespace ::juce::gl;

            if (glGetUniformLocation (shader.getProgramID(), uniformName) < 0)
                return nullptr;

            return new OpenGLShaderProgram::Uniform (shader, uniformName);
        }
    };

    //==============================================================================
    /** This loads a 3D model from an OBJ file and converts it into some vertex buffers
        that we can draw.
    */
    struct Shape
    {
        Shape()
        {
            /// important:
            // always double check that the .obj file ONLY has vertex information.
            // some modeling software includes comments and mtl instructions that cause the file to not load with juce.
            File obj_file = File::getCurrentWorkingDirectory().getChildFile("assets/teapot.obj");

            //auto obj_str = String(BinaryData::teapot_obj, BinaryData::teapot_objSize);
            auto obj_str = String(BinaryData::spiky_obj, BinaryData::spiky_objSize);

            String spiky_obj = String::createStringFromData(BinaryData::spiky_obj, BinaryData::spiky_objSize);
            // okay so this breaks everything because it loads from JUCE's resource library.
            // which we are not accessing our path kinda looks like JUCE/resources/assets/assets/horse.obj which doesn't exist.
//            if (shapeFile.load (loadEntireAssetIntoString ("assets/horse.obj")).wasOk())
//                //DBG("horse asset found");
//                for (auto* s : shapeFile.shapes)
//                    vertexBuffers.add (new VertexBuffer (*s));
            
//            if(shapeFile.load(obj_file).wasOk()){
//                for (auto* s : shapeFile.shapes)
//                    vertexBuffers.add (new VertexBuffer (*s));
//            }
            
            
            
            
            
            if(shapeFile.load(obj_str).wasOk()){
                for (auto* s : shapeFile.shapes)
                    vertexBuffers.add (new VertexBuffer (*s));
            }
        }

        void draw (Attributes& attributes)
        {
            using namespace ::juce::gl;

            for (auto* vertexBuffer : vertexBuffers)
            {
                vertexBuffer->bind();

                attributes.enable();
                glDrawElements (GL_TRIANGLES, vertexBuffer->numIndices, GL_UNSIGNED_INT, nullptr);
                attributes.disable();
            }
        }

    private:
        struct VertexBuffer
        {
            explicit VertexBuffer (WavefrontObjFile::Shape& shape)
            {
                using namespace ::juce::gl;

                numIndices = shape.mesh.indices.size();

                glGenBuffers (1, &vertexBuffer);
                glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);

                Array<Vertex> vertices;
                // Color palette: #D62828 (vibrant red)
                Colour modelColor = Colour::fromRGB(214, 40, 40);
                createVertexListFromMesh (shape.mesh, vertices, modelColor);

                glBufferData (GL_ARRAY_BUFFER, vertices.size() * (int) sizeof (Vertex),
                              vertices.getRawDataPointer(), GL_STATIC_DRAW);

                glGenBuffers (1, &indexBuffer);
                glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
                glBufferData (GL_ELEMENT_ARRAY_BUFFER, numIndices * (int) sizeof (juce::uint32),
                                                       shape.mesh.indices.getRawDataPointer(), GL_STATIC_DRAW);
            }

            ~VertexBuffer()
            {
                using namespace ::juce::gl;

                glDeleteBuffers (1, &vertexBuffer);
                glDeleteBuffers (1, &indexBuffer);
            }

            void bind()
            {
                using namespace ::juce::gl;

                glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
                glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
            }

            GLuint vertexBuffer, indexBuffer;
            int numIndices;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VertexBuffer)
        };

        WavefrontObjFile shapeFile;
        OwnedArray<VertexBuffer> vertexBuffers;

        static void createVertexListFromMesh (const WavefrontObjFile::Mesh& mesh, Array<Vertex>& list, Colour colour)
        {
            auto scale = 0.35f;  // Increased from 0.2f to make object bigger
            WavefrontObjFile::TextureCoord defaultTexCoord = { 0.5f, 0.5f };
            WavefrontObjFile::Vertex defaultNormal = { 0.5f, 0.5f, 0.5f };

            for (int i = 0; i < mesh.vertices.size(); ++i)
            {
                auto& v = mesh.vertices.getReference (i);

                auto& n = (i < mesh.normals.size() ? mesh.normals.getReference (i)
                                                   : defaultNormal);

                auto& tc = (i < mesh.textureCoords.size() ? mesh.textureCoords.getReference (i)
                                                          : defaultTexCoord);

                list.add ({ { scale * v.x, scale * v.y, scale * v.z, },
                            { scale * n.x, scale * n.y, scale * n.z, },
                            { colour.getFloatRed(), colour.getFloatGreen(), colour.getFloatBlue(), colour.getFloatAlpha() },
                            { tc.x, tc.y } });
            }
        }
    };

    //==============================================================================
    struct ShaderPreset
    {
        const char* name;
        const char* vertexShader;
        const char* fragmentShader;
    };

    static Array<ShaderPreset> getPresets()
    {
        #define SHADER_DEMO_HEADER \
            "/*  This is a live OpenGL Shader demo.\n" \
            "    Edit the shader program below and it will be \n" \
            "    compiled and applied to the model above!\n" \
            "*/\n\n"

        ShaderPreset presets[] =
        {
            {
                "Texture + Lighting",

                SHADER_DEMO_HEADER
                "attribute vec4 position;\n"
                "attribute vec4 normal;\n"
                "attribute vec4 sourceColour;\n"
                "attribute vec2 textureCoordIn;\n"
                "\n"
                "uniform mat4 projectionMatrix;\n"
                "uniform mat4 viewMatrix;\n"
                "uniform vec4 lightPosition;\n"
                "\n"
                "varying vec4 destinationColour;\n"
                "varying vec2 textureCoordOut;\n"
                "varying float lightIntensity;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    destinationColour = sourceColour;\n"
                "    textureCoordOut = textureCoordIn;\n"
                "\n"
                "    vec4 light = viewMatrix * lightPosition;\n"
                "    lightIntensity = dot (light, normal);\n"
                "\n"
                "    gl_Position = projectionMatrix * viewMatrix * position;\n"
                "}\n",

                SHADER_DEMO_HEADER
               #if JUCE_OPENGL_ES
                "varying lowp vec4 destinationColour;\n"
                "varying lowp vec2 textureCoordOut;\n"
                "varying highp float lightIntensity;\n"
               #else
                "varying vec4 destinationColour;\n"
                "varying vec2 textureCoordOut;\n"
                "varying float lightIntensity;\n"
               #endif
                "\n"
                "uniform sampler2D demoTexture;\n"
                "\n"
                "void main()\n"
                "{\n"
               #if JUCE_OPENGL_ES
                "   highp float l = max (0.3, lightIntensity * 0.3);\n"
                "   highp vec4 colour = vec4 (l, l, l, 1.0);\n"
               #else
                "   float l = max (0.3, lightIntensity * 0.3);\n"
                "   vec4 colour = vec4 (l, l, l, 1.0);\n"
               #endif
                "    gl_FragColor = colour * texture2D (demoTexture, textureCoordOut);\n"
                "}\n"
            },

            {
                "Textured",

                SHADER_DEMO_HEADER
                "attribute vec4 position;\n"
                "attribute vec4 sourceColour;\n"
                "attribute vec2 textureCoordIn;\n"
                "\n"
                "uniform mat4 projectionMatrix;\n"
                "uniform mat4 viewMatrix;\n"
                "\n"
                "varying vec4 destinationColour;\n"
                "varying vec2 textureCoordOut;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    destinationColour = sourceColour;\n"
                "    textureCoordOut = textureCoordIn;\n"
                "    gl_Position = projectionMatrix * viewMatrix * position;\n"
                "}\n",

                SHADER_DEMO_HEADER
               #if JUCE_OPENGL_ES
                "varying lowp vec4 destinationColour;\n"
                "varying lowp vec2 textureCoordOut;\n"
               #else
                "varying vec4 destinationColour;\n"
                "varying vec2 textureCoordOut;\n"
               #endif
                "\n"
                "uniform sampler2D demoTexture;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    gl_FragColor = texture2D (demoTexture, textureCoordOut);\n"
                "}\n"
            },

            {
                "Flat Colour",

                SHADER_DEMO_HEADER
                "attribute vec4 position;\n"
                "attribute vec4 sourceColour;\n"
                "attribute vec2 textureCoordIn;\n"
                "\n"
                "uniform mat4 projectionMatrix;\n"
                "uniform mat4 viewMatrix;\n"
                "\n"
                "varying vec4 destinationColour;\n"
                "varying vec2 textureCoordOut;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    destinationColour = sourceColour;\n"
                "    textureCoordOut = textureCoordIn;\n"
                "    gl_Position = projectionMatrix * viewMatrix * position;\n"
                "}\n",

                SHADER_DEMO_HEADER
               #if JUCE_OPENGL_ES
                "varying lowp vec4 destinationColour;\n"
                "varying lowp vec2 textureCoordOut;\n"
               #else
                "varying vec4 destinationColour;\n"
                "varying vec2 textureCoordOut;\n"
               #endif
                "\n"
                "void main()\n"
                "{\n"
                "    gl_FragColor = destinationColour;\n"
                "}\n"
            },

            {
                "Rainbow",

                SHADER_DEMO_HEADER
                "attribute vec4 position;\n"
                "attribute vec4 sourceColour;\n"
                "attribute vec2 textureCoordIn;\n"
                "\n"
                "uniform mat4 projectionMatrix;\n"
                "uniform mat4 viewMatrix;\n"
                "\n"
                "varying vec4 destinationColour;\n"
                "varying vec2 textureCoordOut;\n"
                "\n"
                "varying float xPos;\n"
                "varying float yPos;\n"
                "varying float zPos;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    vec4 v = vec4 (position);\n"
                "    xPos = clamp (v.x, 0.0, 1.0);\n"
                "    yPos = clamp (v.y, 0.0, 1.0);\n"
                "    zPos = clamp (v.z, 0.0, 1.0);\n"
                "    gl_Position = projectionMatrix * viewMatrix * position;\n"
                "}",

                SHADER_DEMO_HEADER
               #if JUCE_OPENGL_ES
                "varying lowp vec4 destinationColour;\n"
                "varying lowp vec2 textureCoordOut;\n"
                "varying lowp float xPos;\n"
                "varying lowp float yPos;\n"
                "varying lowp float zPos;\n"
               #else
                "varying vec4 destinationColour;\n"
                "varying vec2 textureCoordOut;\n"
                "varying float xPos;\n"
                "varying float yPos;\n"
                "varying float zPos;\n"
               #endif
                "\n"
                "void main()\n"
                "{\n"
                "    gl_FragColor = vec4 (xPos, yPos, zPos, 1.0);\n"
                "}"
            },

            {
                "Changing Colour",

                SHADER_DEMO_HEADER
                "attribute vec4 position;\n"
                "attribute vec2 textureCoordIn;\n"
                "\n"
                "uniform mat4 projectionMatrix;\n"
                "uniform mat4 viewMatrix;\n"
                "\n"
                "varying vec2 textureCoordOut;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    textureCoordOut = textureCoordIn;\n"
                "    gl_Position = projectionMatrix * viewMatrix * position;\n"
                "}\n",

                SHADER_DEMO_HEADER
                "#define PI 3.1415926535897932384626433832795\n"
                "\n"
               #if JUCE_OPENGL_ES
                "precision mediump float;\n"
                "varying lowp vec2 textureCoordOut;\n"
               #else
                "varying vec2 textureCoordOut;\n"
               #endif
                "uniform float bouncingNumber;\n"
                "\n"
                "void main()\n"
                "{\n"
                "   float b = bouncingNumber;\n"
                "   float n = b * PI * 2.0;\n"
                "   float sn = (sin (n * textureCoordOut.x) * 0.5) + 0.5;\n"
                "   float cn = (sin (n * textureCoordOut.y) * 0.5) + 0.5;\n"
                "\n"
                "   vec4 col = vec4 (b, sn, cn, 1.0);\n"
                "   gl_FragColor = col;\n"
                "}\n"
            },

            {
                "Simple Light",

                SHADER_DEMO_HEADER
                "attribute vec4 position;\n"
                "attribute vec4 normal;\n"
                "\n"
                "uniform mat4 projectionMatrix;\n"
                "uniform mat4 viewMatrix;\n"
                "uniform vec4 lightPosition;\n"
                "\n"
                "varying float lightIntensity;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    vec4 light = viewMatrix * lightPosition;\n"
                "    lightIntensity = dot (light, normal);\n"
                "\n"
                "    gl_Position = projectionMatrix * viewMatrix * position;\n"
                "}\n",

                SHADER_DEMO_HEADER
               #if JUCE_OPENGL_ES
                "varying highp float lightIntensity;\n"
               #else
                "varying float lightIntensity;\n"
               #endif
                "\n"
                "void main()\n"
                "{\n"
               #if JUCE_OPENGL_ES
                "   highp float l = lightIntensity * 0.25;\n"
                "   highp vec4 colour = vec4 (l, l, l, 1.0);\n"
               #else
                "   float l = lightIntensity * 0.25;\n"
                "   vec4 colour = vec4 (l, l, l, 1.0);\n"
               #endif
                "\n"
                "    gl_FragColor = colour;\n"
                "}\n"
            },

            {
                "Flattened",

                SHADER_DEMO_HEADER
                "attribute vec4 position;\n"
                "attribute vec4 normal;\n"
                "\n"
                "uniform mat4 projectionMatrix;\n"
                "uniform mat4 viewMatrix;\n"
                "uniform vec4 lightPosition;\n"
                "\n"
                "varying float lightIntensity;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    vec4 light = viewMatrix * lightPosition;\n"
                "    lightIntensity = dot (light, normal);\n"
                "\n"
                "    vec4 v = vec4 (position);\n"
                "    v.z = v.z * 0.1;\n"
                "\n"
                "    gl_Position = projectionMatrix * viewMatrix * v;\n"
                "}\n",

                SHADER_DEMO_HEADER
               #if JUCE_OPENGL_ES
                "varying highp float lightIntensity;\n"
               #else
                "varying float lightIntensity;\n"
               #endif
                "\n"
                "void main()\n"
                "{\n"
               #if JUCE_OPENGL_ES
                "   highp float l = lightIntensity * 0.25;\n"
                "   highp vec4 colour = vec4 (l, l, l, 1.0);\n"
               #else
                "   float l = lightIntensity * 0.25;\n"
                "   vec4 colour = vec4 (l, l, l, 1.0);\n"
               #endif
                "\n"
                "    gl_FragColor = colour;\n"
                "}\n"
            },

            {
                "Toon Shader",

                SHADER_DEMO_HEADER
                "attribute vec4 position;\n"
                "attribute vec4 normal;\n"
                "\n"
                "uniform mat4 projectionMatrix;\n"
                "uniform mat4 viewMatrix;\n"
                "uniform vec4 lightPosition;\n"
                "\n"
                "varying float lightIntensity;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    vec4 light = viewMatrix * lightPosition;\n"
                "    lightIntensity = dot (light, normal);\n"
                "\n"
                "    gl_Position = projectionMatrix * viewMatrix * position;\n"
                "}\n",

                SHADER_DEMO_HEADER
               #if JUCE_OPENGL_ES
                "varying highp float lightIntensity;\n"
               #else
                "varying float lightIntensity;\n"
               #endif
                "\n"
                "void main()\n"
                "{\n"
               #if JUCE_OPENGL_ES
                "    highp float intensity = lightIntensity * 0.5;\n"
                "    highp vec4 colour;\n"
               #else
                "    float intensity = lightIntensity * 0.5;\n"
                "    vec4 colour;\n"
               #endif
                "\n"
                "    if (intensity > 0.95)\n"
                "        colour = vec4 (1.0, 0.5, 0.5, 1.0);\n"
                "    else if (intensity > 0.5)\n"
                "        colour  = vec4 (0.6, 0.3, 0.3, 1.0);\n"
                "    else if (intensity > 0.25)\n"
                "        colour  = vec4 (0.4, 0.2, 0.2, 1.0);\n"
                "    else\n"
                "        colour  = vec4 (0.2, 0.1, 0.1, 1.0);\n"
                "\n"
                "    gl_FragColor = colour;\n"
                "}\n"
            }
        };

        return Array<ShaderPreset> (presets, numElementsInArray (presets));
    }
    
    struct default_shader final {
        static inline const char* vertexShader =
            SHADER_DEMO_HEADER
            "attribute vec4 position;\n"
            "attribute vec4 normal;\n"
            "\n"
            "uniform mat4 projectionMatrix;\n"
            "uniform mat4 viewMatrix;\n"
            "uniform vec4 lightPosition;\n"
            "\n"
            "varying float lightIntensity;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    vec4 light = viewMatrix * lightPosition;\n"
            "    lightIntensity = dot (light, normal);\n"
            "\n"
            "    gl_Position = projectionMatrix * viewMatrix * position;\n"
            "}\n";
        
        static inline const char* fragmentShader =
            SHADER_DEMO_HEADER
            #if JUCE_OPENGL_ES
            "varying highp float lightIntensity;\n"
            #else
            "varying float lightIntensity;\n"
            #endif
            "\n"
            "void main()\n"
            "{\n"
            #if JUCE_OPENGL_ES
            "    highp float intensity = lightIntensity * 0.5;\n"
            "    highp vec4 colour;\n"
            #else
            "    float intensity = lightIntensity * 0.5;\n"
            "    vec4 colour;\n"
            #endif
            "\n"
            "    if (intensity > 0.95)\n"
            "        colour = vec4 (1.0, 0.5, 0.5, 1.0);\n"
            "    else if (intensity > 0.5)\n"
            "        colour  = vec4 (0.6, 0.3, 0.3, 1.0);\n"
            "    else if (intensity > 0.25)\n"
            "        colour  = vec4 (0.4, 0.2, 0.2, 1.0);\n"
            "    else\n"
            "        colour  = vec4 (0.2, 0.1, 0.1, 1.0);\n"
            "\n"
            "    gl_FragColor = colour;\n"
            "}\n";
        
        static const char* get_fragment_shader() {
            return fragmentShader;
        }
        static const char* get_vertex_shader() {
            return vertexShader;
        }
    };
    
    static ShaderPreset get_rainbow_preset() {
        return {
            "Rainbow",
            
            SHADER_DEMO_HEADER
            "attribute vec4 position;\n"
            "attribute vec4 sourceColour;\n"
            "attribute vec2 textureCoordIn;\n"
            "\n"
            "uniform mat4 projectionMatrix;\n"
            "uniform mat4 viewMatrix;\n"
            "\n"
            "varying vec4 destinationColour;\n"
            "varying vec2 textureCoordOut;\n"
            "\n"
            "varying float xPos;\n"
            "varying float yPos;\n"
            "varying float zPos;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    vec4 v = vec4 (position);\n"
            "    xPos = clamp (v.x, 0.0, 1.0);\n"
            "    yPos = clamp (v.y, 0.0, 1.0);\n"
            "    zPos = clamp (v.z, 0.0, 1.0);\n"
            "    gl_Position = projectionMatrix * viewMatrix * position;\n"
            "}",
            
            SHADER_DEMO_HEADER
           #if JUCE_OPENGL_ES
            "varying lowp vec4 destinationColour;\n"
            "varying lowp vec2 textureCoordOut;\n"
            "varying lowp float xPos;\n"
            "varying lowp float yPos;\n"
            "varying lowp float zPos;\n"
           #else
            "varying vec4 destinationColour;\n"
            "varying vec2 textureCoordOut;\n"
            "varying float xPos;\n"
            "varying float yPos;\n"
            "varying float zPos;\n"
           #endif
            "\n"
            "void main()\n"
            "{\n"
            "    gl_FragColor = vec4 (xPos, yPos, zPos, 1.0);\n"
            "}"
        };
    }
    static ShaderPreset get_toon_preset(){
        return {
            "Toon",

            SHADER_DEMO_HEADER
            "attribute vec4 position;\n"
            "attribute vec4 normal;\n"
            "\n"
            "uniform mat4 projectionMatrix;\n"
            "uniform mat4 viewMatrix;\n"
            "uniform mat4 modelMatrix;\n"
            "uniform vec4 lightPosition;\n"
            "\n"
            "varying float lightIntensity;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    // Transform position to world space\n"
            "    vec4 worldPos = viewMatrix * modelMatrix * position;\n"
            "    \n"
            "    // Transform normal to world space (using model matrix only for rotation)\n"
            "    vec4 worldNormal = modelMatrix * normal;\n"
            "    \n"
            "    // Light is in world space, calculate direction from surface to light\n"
            "    vec4 lightDir = normalize(lightPosition - worldPos);\n"
            "    \n"
            "    // Calculate lighting based on light distance (F/R keys)\n"
            "    float distance = length(lightPosition - worldPos);\n"
            "    float attenuation = 1.0 / (1.0 + distance * 0.05);\n"
            "    \n"
            "    lightIntensity = dot(normalize(worldNormal), lightDir) * attenuation;\n"
            "\n"
            "    gl_Position = projectionMatrix * viewMatrix * modelMatrix * position;\n"
            "}\n",
            
            SHADER_DEMO_HEADER
            #if JUCE_OPENGL_ES
            "varying highp float lightIntensity;\n"
            #else
            "varying float lightIntensity;\n"
            #endif
            "\n"
            "void main()\n"
            "{\n"
            #if JUCE_OPENGL_ES
            "    highp float intensity = lightIntensity * 0.5;\n"
            "    highp vec4 colour;\n"
            #else
            "    float intensity = lightIntensity * 0.5;\n"
            "    vec4 colour;\n"
            #endif
            "\n"
            "    if (intensity > 0.95)\n"
            "        colour = vec4 (0.96, 0.96, 0.96, 1.0);\n"
            "    else if (intensity > 0.85)\n"
            "        colour = vec4(0.90, 0.90, 0.90, 1.0);\n"
            "    else if (intensity > 0.75)\n"
            "        colour = vec4(0.87, 0.87, 0.87, 1.0);\n"
            "    else if (intensity > 0.65)\n"
            "        colour = vec4(0.67, 0.67, 0.67, 1.0);\n"
            "    else if (intensity > 0.45)\n"
            "        colour = vec4(0.57, 0.57, 0.57, 1.0);\n"
            "    else if (intensity > 0.25)\n"
            "        colour  = vec4 (0.95, 0.31, 0.41, 1.0);\n"
            "    else\n"
            "        colour  = vec4 (0.95, 0.2, 0.3, 1.0);\n"
            "\n"
            "    gl_FragColor = colour;\n"
            "}\n"
        };
    }

    //==============================================================================
    // These classes are used to load textures from the various sources that the demo uses..
    struct DemoTexture
    {
        virtual ~DemoTexture() {}
        virtual bool applyTo (OpenGLTexture&) = 0;

        String name;
    };

    static Image resizeImageToPowerOfTwo (Image image)
    {
        if (! (isPowerOfTwo (image.getWidth()) && isPowerOfTwo (image.getHeight())))
            return image.rescaled (jmin (1024, nextPowerOfTwo (image.getWidth())),
                                   jmin (1024, nextPowerOfTwo (image.getHeight())));

        return image;
    }

    struct BuiltInTexture final : public DemoTexture
    {
        BuiltInTexture (const char* nm, const void* imageData, size_t imageSize)
            : image (resizeImageToPowerOfTwo (ImageFileFormat::loadFrom (imageData, imageSize)))
        {
            name = nm;
        }

        Image image;

        bool applyTo (OpenGLTexture& texture) override
        {
            texture.loadImage (image);
            return false;
        }
    };

    struct TextureFromFile final : public DemoTexture
    {
        TextureFromFile (const File& file)
        {
            name = file.getFileName();
            image = resizeImageToPowerOfTwo (ImageFileFormat::loadFrom (file));
        }

        Image image;

        bool applyTo (OpenGLTexture& texture) override
        {
            texture.loadImage (image);
            return false;
        }
    };

    struct TextureFromAsset final : public DemoTexture
    {
        TextureFromAsset (const char* assetName)
        {
            name = assetName;
            image = resizeImageToPowerOfTwo (getImageFromAssets (assetName));
        }

        Image image;

        bool applyTo (OpenGLTexture& texture) override
        {
            texture.loadImage (image);
            return false;
        }
    };
};

class OpenGLOut final :
    public Component,
    private OpenGLRenderer

{ // final means can't be inherited from

public:
    OpenGLOut(){
        if (auto* peer = getPeer()){
            peer->setCurrentRenderingEngine(0);
        }
        OpenGLUtils::ShaderPreset rainbow = OpenGLUtils::get_toon_preset();
        if(shader.get() == nullptr){
            setShaderProgram(rainbow.vertexShader, rainbow.fragmentShader);
            //setShaderProgram(OpenGLUtils::default_shader::get_vertex_shader(),OpenGLUtils::default_shader::get_fragment_shader());
            DBG("shader program set successfully.");
            // newVertexShader = OpenGLUtils::getPresets()[4];
        }
        setOpaque(true);
        overlay.reset (new class overlay (*this));
        addAndMakeVisible(overlay.get());
        openGLContext.setOpenGLVersionRequired(OpenGLContext::openGL3_2);
        openGLContext.setRenderer(this);
        openGLContext.attachTo(*this);
        openGLContext.setContinuousRepainting(true);
        
        setSize(500,300);
    }
    ~OpenGLOut() override{
        openGLContext.detach();
        openGLContext.setRenderer(nullptr);
    }
    //void setProcessor(TheHorsePluginAudioProcessor& proc){processor = &proc;}
    void newOpenGLContextCreated() override {
        freeAllContextObjects();
    }
    void openGLContextClosing() override{
        freeAllContextObjects();
        if(lastTexture != nullptr){
            setTexture(lastTexture);
        }
    }
    void setTexture(OpenGLUtils::DemoTexture* t){
        lastTexture = textureToUse = t;
    }
    void setShaderProgram(const String& vertexShader, const String& fragmentShader){
        const ScopedLock lock (shaderMutex);
        newVertexShader = vertexShader;
        newFragmentShader = fragmentShader;
    }
    void freeAllContextObjects(){
        shape.reset();
        shader.reset();
        attributes.reset();
        uniforms.reset();
        texture.release();
    }
    
    std::function<void(float)> update_callback;

    // SHADOW PLANE - Draw a dark ground plane beneath the object
    void drawShadowPlane() {
        using namespace ::juce::gl;

        // Simple quad vertices for the ground plane
        const float planeSize = 20.0f;
        const float yPos = -1.5f;  // Sit below the horse

        // Shadow color: darker version of background (#001a28)
        float shadowVerts[] = {
            // Position (X, Y, Z)         Color (R, G, B, A)
            -planeSize, yPos, -planeSize,  0.0f, 0.1f, 0.16f, 0.8f,
             planeSize, yPos, -planeSize,  0.0f, 0.1f, 0.16f, 0.8f,
             planeSize, yPos,  planeSize,  0.0f, 0.1f, 0.16f, 0.8f,
            -planeSize, yPos,  planeSize,  0.0f, 0.1f, 0.16f, 0.8f
        };

        unsigned int indices[] = {0, 1, 2, 0, 2, 3};

        GLuint vbo, ibo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ibo);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(shadowVerts), shadowVerts, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Set up vertex attributes (position + color)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Apply view/projection matrix (same as main object)
        auto projectionMatrix = getProjectionMatrix();
        auto viewMatrix = getViewMatrix();

        // Draw the plane
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // Cleanup
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ibo);
    }

    // basically the onpaint() function.
    // called everytime the opengl canvas needs to render
    // calling the update rotation everytime we draw and pass it to processor.
    // actually a really simple solution to the multi threading.
    void renderOpenGL() override {
        using namespace ::juce::gl;
        const ScopedLock lock (mutex);
        //
        double time = 15 * std::sin(RelativeTime().inMilliseconds() / 500);
        
        jassert(OpenGLHelpers::isContextActive());
        
        auto desktopScale = (float) openGLContext.getRenderingScale();
        
        OpenGLHelpers::clear(getUIColourIfAvailable(LookAndFeel_V4::ColourScheme::UIColour::windowBackground,Colours::whitesmoke));
        
        if (textureToUse != nullptr)
            if (! textureToUse->applyTo(texture))
                textureToUse = nullptr;
        
        // oh oh okay here's where paint methods start...
        // draw background
        // i chose not to put any 2D assets in the background so uh no background drawing lol.
        updateShader();
        // the demo uses a GUI to select shaders and waits for a shader to be selected whether that's from the default constructor of the gui or from the user.
        // i am not using that gui So i need to come up with a way to fix that.
        if(shader.get() == nullptr){
            DBG("there's no shader");
            return;
        }
        // White background
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // White
        // gotta initialize a ton of stuff to make sure GL is ready to do everything correctyl.
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // alright bruh are we fr with these GL objects. this language/tool is hella confusing I get why nobody uses it.
        glActiveTexture(GL_TEXTURE0);
        // Imma get goated at this language/tool >:)
        if(! openGLContext.isCoreProfile())
            glEnable(GL_TEXTURE_2D);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0,0,
                   roundToInt (desktopScale * (float) bounds.getWidth()),
                   roundToInt (desktopScale * (float) bounds.getHeight()));
        texture.bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        shader->use();
        
        if(uniforms->projectionMatrix != nullptr)
            uniforms->projectionMatrix->setMatrix4(getProjectionMatrix().mat,1,false);

        if (uniforms->viewMatrix != nullptr)
            uniforms->viewMatrix->setMatrix4 (getViewMatrix().mat, 1, false);

        if (uniforms->modelMatrix != nullptr)
            uniforms->modelMatrix->setMatrix4 (getModelMatrix().mat, 1, false);

        if (uniforms->texture != nullptr)
            uniforms->texture->set ((GLint) 0);

        // DYNAMIC LIGHT POSITION - Animate X, use member variables for Y/Z
        //lightX = static_cast<float>(time);  // Animate X position based on time
        if (uniforms->lightPosition != nullptr)
            uniforms->lightPosition->set (lightX, lightY, lightZ, 0.0f);

        // Update audio processor with light position for reverb control
        if (light_callback)
            light_callback(lightX, lightY, lightZ);

        shape->draw(*attributes);

        // DRAW SHADOW PLANE - Disabled for performance (was creating buffers every frame!)
        // drawShadowPlane();

        // unbind GL BUFFER remember when you forgot to do that that one time lol
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // CONTINUOUS WASD MOVEMENT - Update position every frame based on key states
        const float moveSpeed = 0.05f; // Units per frame (~60fps = ~3 units/sec) - Smooth and controlled
        const int64 keyTimeout = 100;  // Auto-release if no event for 100ms
        int64 currentTime = Time::currentTimeMillis();
        bool positionChanged = false;

        // Auto-release keys that haven't been updated recently (key was released)
        if (keyStates.wPressed.load() && (currentTime - keyStates.wLastTime.load()) > keyTimeout)
            keyStates.wPressed = false;
        if (keyStates.sPressed.load() && (currentTime - keyStates.sLastTime.load()) > keyTimeout)
            keyStates.sPressed = false;
        if (keyStates.aPressed.load() && (currentTime - keyStates.aLastTime.load()) > keyTimeout)
            keyStates.aPressed = false;
        if (keyStates.dPressed.load() && (currentTime - keyStates.dLastTime.load()) > keyTimeout)
            keyStates.dPressed = false;
        if (keyStates.qPressed.load() && (currentTime - keyStates.qLastTime.load()) > keyTimeout)
            keyStates.qPressed = false;
        if (keyStates.ePressed.load() && (currentTime - keyStates.eLastTime.load()) > keyTimeout)
            keyStates.ePressed = false;
        if (keyStates.fPressed.load() && (currentTime - keyStates.fLastTime.load()) > keyTimeout)
            keyStates.fPressed = false;
        if (keyStates.rPressed.load() && (currentTime - keyStates.rLastTime.load()) > keyTimeout)
            keyStates.rPressed = false;

        // Apply movement based on active keys with bounds checking
        if (keyStates.wPressed.load()) {
            posZ += moveSpeed;  // Move forward
            positionChanged = true;
        }
        if (keyStates.sPressed.load()) {
            posZ -= moveSpeed;  // Move backward
            positionChanged = true;
        }
        if (keyStates.aPressed.load()) {
            posX -= moveSpeed;  // Move left
            positionChanged = true;
        }
        if (keyStates.dPressed.load()) {
            posX += moveSpeed;  // Move right
            positionChanged = true;
        }
        if (keyStates.qPressed.load()) {
            posY += moveSpeed;  // Move up
            positionChanged = true;
        }
        if (keyStates.ePressed.load()) {
            posY -= moveSpeed;  // Move down
            positionChanged = true;
        }

        // CLAMP POSITION to keep object visible on screen
        // These bounds match the reverb parameter ranges
        posX = juce::jlimit(-30.0f, 30.0f, posX);   // Left/Right bounds
        posY = juce::jlimit(-10.0f, 20.0f, posY);   // Up/Down bounds
        posZ = juce::jlimit(-25.0f, 5.0f, posZ);    // Forward/Back bounds

        // Notify audio processor if position changed
        if (positionChanged && position_callback) {
            position_callback(posX, posY, posZ);
        }

        // F/R keys control light X position
        bool lightChanged = false;
        if (keyStates.fPressed.load()) {
            lightZ += moveSpeed * 2.0f;  // Move light farther = more wet
            lightChanged = true;
        }
        if (keyStates.rPressed.load()) {
            lightZ -= moveSpeed * 2.0f;  // Move light closer = more dry
            lightChanged = true;
        }

        // CLAMP LIGHT POSITION to reasonable bounds
        lightZ = juce::jlimit(-50.0f, 50.0f, lightX);

        // Notify audio processor if light changed
        if (lightChanged && light_callback) {
            light_callback(lightX, lightY, lightZ);
        }

        // the docs has a mutex check before this but i'm not using the control bay they use so not doing the mutex check.
        if (! overlay->isMouseButtonDownThreadSafe()){
            rotation += (float) rotationSpeed;
        }
//        if(update_callback)
//            update_callback(juce::jmap(sum_rotation(),(float)-3.14f, (float) 3.14f, (float) 0.001, (float) 1.0f));

        // there needs to be a seperate callback system to basically run the rotation calculation
        // in the background while the display is closed.
        // that's the #1 problem at the moment.
        // and the artifacting...
        // ie: if(!windowIsOpen()) process rotation outside of render loop.
        if(update_callback)
            update_callback(std::abs(sum_rotation()));
        /*
        if(processor){
            static_cast<TheHorsePluginAudioProcessor*>(processor)->update_dt(rotation);
        }*/
    }
    
    Matrix3D<float> getProjectionMatrix() const {
        const ScopedLock lock (mutex);
        
        auto w = 1.0f / (scale + 0.1f);
        auto h = w * bounds.toFloat().getAspectRatio(false);
        
        return Matrix3D<float>::fromFrustum (-w, w, -h, h, 4.0f, 30.0f);
    }
    
    // need to have a way to communicate between the DGO rotation matrix and audio processor for realtime "rotation audio."
    Matrix3D<float> getViewMatrix() const {
        const ScopedLock lock (mutex);

        // View matrix only handles translation and draggable orientation - NO automatic rotation
        // This keeps the light position static in world space
        auto viewMatrix = Matrix3D<float>::fromTranslation ({posX, posY, posZ}) * draggableOrientation.getRotationMatrix();

        return viewMatrix;
    }

    // Model matrix handles the object's rotation
    Matrix3D<float> getModelMatrix() const {
        const ScopedLock lock (mutex);

        auto rotationMatrix = Matrix3D<float>::rotation ({rotation, rotation, -0.3f});

        return rotationMatrix;
    }
    
    void paint (Graphics&) override {}
    
    void resized() override {
        const ScopedLock lock (mutex);
        
        bounds = getLocalBounds();
        overlay->setBounds(bounds);
        draggableOrientation.setViewport(bounds);
    }
    float sum_rotation() {
        auto r_matrix = draggableOrientation.getRotationMatrix();
        float rX = std::atan2(r_matrix.mat[6], r_matrix.mat[10]);
        //DBG(rX + rotation);
        return rX + rotation;
    }

    // WASD POSITION CONTROL - Update spatial position and notify audio processor
    void movePosition(float deltaX, float deltaY, float deltaZ) {
        const ScopedLock lock (mutex);
        posX += deltaX;
        posY += deltaY;
        posZ += deltaZ;

        // Notify audio processor of position change
        if (position_callback)
            position_callback(posX, posY, posZ);
    }

    // Callback to update audio processor with position
    std::function<void(float, float, float)> position_callback;

    // Callback to update audio processor with light position
    std::function<void(float, float, float)> light_callback;

    //
    float rotation = 1.3f;
    std::unique_ptr<OpenGLShaderProgram> shader;
    std::unique_ptr<OpenGLUtils::Shape> shape;
    std::unique_ptr<OpenGLUtils::Attributes> attributes;
    std::unique_ptr<OpenGLUtils::Uniforms> uniforms;

    OpenGLTexture texture;
    OpenGLUtils::DemoTexture* textureToUse = nullptr;
    OpenGLUtils::DemoTexture* lastTexture = nullptr;

    CriticalSection shaderMutex;
    String newVertexShader, newFragmentShader, statusText;

    OpenGLContext openGLContext;

    Rectangle<int> bounds;
    Draggable3DOrientation draggableOrientation;
    bool doBackgroundDrawing = false;
    float scale = 1.f, rotationSpeed = 0.002f;  // Slowed down from 0.01f - 5x slower
    CriticalSection mutex;

    // SPATIAL POSITION FOR WASD CONTROL (starting position lower)
    float posX = 0.0f;
    float posY = 0.0f;  // Centered starting position
    float posZ = -10.0f;

    // LIGHT POSITION FOR REVERB CONTROL
    float lightX = 0.0f;
    float lightY = 10.0f;
    float lightZ = 15.0f;

    // KEY STATE TRACKING FOR CONTINUOUS MOVEMENT
    struct KeyStates {
        std::atomic<bool> wPressed{false};
        std::atomic<bool> sPressed{false};
        std::atomic<bool> aPressed{false};
        std::atomic<bool> dPressed{false};
        std::atomic<bool> qPressed{false};
        std::atomic<bool> ePressed{false};
        std::atomic<bool> fPressed{false};  // Light farther (more wet)
        std::atomic<bool> rPressed{false};  // Light closer (more dry)

        // Timestamps for auto-release detection (in milliseconds)
        std::atomic<int64> wLastTime{0};
        std::atomic<int64> sLastTime{0};
        std::atomic<int64> aLastTime{0};
        std::atomic<int64> dLastTime{0};
        std::atomic<int64> qLastTime{0};
        std::atomic<int64> eLastTime{0};
        std::atomic<int64> fLastTime{0};
        std::atomic<int64> rLastTime{0};
    };
    KeyStates keyStates;
    //
    
    void updateShader(){
        const ScopedLock lock (shaderMutex);
        if(newVertexShader.isNotEmpty() || newFragmentShader.isNotEmpty()){
            std::unique_ptr<OpenGLShaderProgram> newShader (new OpenGLShaderProgram (openGLContext));
            
            if(newShader->addVertexShader(OpenGLHelpers::translateVertexShaderToV3(newVertexShader)) && newShader->addFragmentShader(OpenGLHelpers::translateFragmentShaderToV3(newFragmentShader)) && newShader->link()){
                shape.reset();
                attributes.reset();
                uniforms.reset();
                
                shader.reset(newShader.release());
                shader->use();
                
                shape.reset(new OpenGLUtils::Shape());
                attributes.reset(new OpenGLUtils::Attributes(*shader));
                uniforms.reset(new OpenGLUtils::Uniforms(*shader));
            }
        }
    }
    
    class overlay : public Component, private Timer {
    public:
        overlay (OpenGLOut& ogl) : gl(ogl), textIndex(0) {
            image = juce::ImageCache::getFromMemory(BinaryData::hud_png, BinaryData::hud_pngSize);
            lines = juce::ImageCache::getFromMemory(BinaryData::lines_png, BinaryData::lines_pngSize);
            turbine = juce::ImageCache::getFromMemory(BinaryData::turbine_png, BinaryData::turbine_pngSize);
            texture = juce::ImageCache::getFromMemory(BinaryData::texture_png, BinaryData::texture_pngSize);
            setWantsKeyboardFocus(true);

            displayText = "MADE WITH <3 BY PUBLIC SERVICES ";

            // Start timer for cycling text (8 fps for character cycling effect)
            startTimer(125);  // ~8 fps
        }

        void parentHierarchyChanged() override {
            // Grab keyboard focus as soon as we're added to the component hierarchy
            if (isShowing())
                grabKeyboardFocus();
        }
        bool isMouseButtonDownThreadSafe() const { return buttonDown; }
        void mouseDown (const MouseEvent& e) override {
            const ScopedLock lock (gl.mutex);
            gl.draggableOrientation.mouseDown(e.getPosition());
            buttonDown = true;
            grabKeyboardFocus(); // Grab focus for WASD controls
        }
        void mouseDrag (const MouseEvent& e) override {
            const ScopedLock lock (gl.mutex);
            gl.draggableOrientation.mouseDrag(e.getPosition());
        }
        void mouseUp (const MouseEvent&) override {
            buttonDown = false;
        }

        // WASD KEYBOARD CONTROL - Track key states for continuous movement
        bool keyPressed(const KeyPress& key) override {
            int64 currentTime = Time::currentTimeMillis();

            if (key == KeyPress('w') || key == KeyPress('W')) {
                gl.keyStates.wPressed = true;
                gl.keyStates.wLastTime = currentTime;
                return true;
            }
            if (key == KeyPress('s') || key == KeyPress('S')) {
                gl.keyStates.sPressed = true;
                gl.keyStates.sLastTime = currentTime;
                return true;
            }
            if (key == KeyPress('a') || key == KeyPress('A')) {
                gl.keyStates.aPressed = true;
                gl.keyStates.aLastTime = currentTime;
                return true;
            }
            if (key == KeyPress('d') || key == KeyPress('D')) {
                gl.keyStates.dPressed = true;
                gl.keyStates.dLastTime = currentTime;
                return true;
            }
            if (key == KeyPress('q') || key == KeyPress('Q')) {
                gl.keyStates.qPressed = true;
                gl.keyStates.qLastTime = currentTime;
                return true;
            }
            if (key == KeyPress('e') || key == KeyPress('E')) {
                gl.keyStates.ePressed = true;
                gl.keyStates.eLastTime = currentTime;
                return true;
            }
            if (key == KeyPress('f') || key == KeyPress('F')) {
                gl.keyStates.fPressed = true;
                gl.keyStates.fLastTime = currentTime;
                return true;
            }
            if (key == KeyPress('r') || key == KeyPress('R')) {
                gl.keyStates.rPressed = true;
                gl.keyStates.rLastTime = currentTime;
                return true;
            }

            return Component::keyPressed(key);
        }

        void modifierKeysChanged(const ModifierKeys& modifiers) override {
            // Check if any keys were released by polling current key states
            // This is a workaround since JUCE doesn't provide keyReleased with the specific key

            // For now, we'll rely on the key repeat stopping when released
            // The continuous movement in renderOpenGL will naturally stop when key state is false
        }

        void focusLost(FocusChangeType cause) override {
            // Clear all key states when focus is lost
            gl.keyStates.wPressed = false;
            gl.keyStates.sPressed = false;
            gl.keyStates.aPressed = false;
            gl.keyStates.dPressed = false;
            gl.keyStates.qPressed = false;
            gl.keyStates.ePressed = false;
            gl.keyStates.fPressed = false;
            gl.keyStates.rPressed = false;
        }

        // Handle key releases - JUCE will call keyPressed repeatedly while held
        // We need to detect when keys stop being pressed
        void handleKeyRelease() {
            // This will be called periodically to check if keys are still down
            // Since JUCE repeats keyPressed events, if we don't get events, keys are released
        }

        void timerCallback() override {
            // Cycle through text character by character
            textIndex++;
            if (textIndex >= displayText.length())
                textIndex = 0;
            repaint();
        }

        void paint (juce::Graphics& g) override {
            // Draw texture overlay stretched across ENTIRE screen with higher alpha
            if (texture.isValid()) {
                g.setOpacity(0.5f);  // 50% opacity - more visible
                g.drawImageWithin(texture, 0, 0, weight, height,
                                 juce::RectanglePlacement::stretchToFit);
                g.setOpacity(1.0f);  // Reset opacity
            }

            // Draw cycling 10-character display (LED-style)
            g.setColour(Colour::fromRGB(214, 40, 40));  // #D62828 (red)
            g.setFont(Font("Helvetica", 12.0f, Font::plain));  // Helvetica

            // Get 10 characters starting from current index, convert to lowercase
            String displayWindow = "";
            for (int i = 0; i < 10; i++) {
                int charIndex = (textIndex + i) % displayText.length();
                displayWindow += String::charToString(displayText[charIndex]).toLowerCase();
            }

            // Position in bottom-right corner
            int textX = weight - 130;  // Right side, wider for 10 chars
            int textY = height - 25;    // Bottom

            g.drawText(displayWindow, textX, textY, 120, 20, Justification::centredLeft);
        }
        void resized() override {
            height = getHeight();
            weight = getWidth();
        }
        OpenGLOut& gl;
    private:
        Image image;
        Image lines;
        Image turbine;
        Image texture;
        int height;
        int weight;
        int textIndex;
        String displayText;
        std::atomic<bool> buttonDown {false};
    };
    std::unique_ptr<overlay> overlay;
private:
    // docs has async update handling here but I'm not putting the gui stuff inside so uh not doing that.
};
