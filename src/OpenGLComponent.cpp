#include "OpenGLComponent.hpp"

using namespace juce::gl;

OpenGLComponent::OpenGLComponent() { 
setOpaque(true);
  openGLContext.setRenderer(this);
  openGLContext.setContinuousRepainting(true);
  openGLContext.attachTo(*this);
  addAndMakeVisible(transparentOverlay);
transparentOverlay.onMouseDragCallback = [this](float x, float y) { updatePositionOffset(x, y); };
}
OpenGLComponent::~OpenGLComponent() { openGLContext.detach(); }
void OpenGLComponent::newOpenGLContextCreated() {
  openGLContext.extensions.glGenBuffers(1, &vbo);
  openGLContext.extensions.glGenBuffers(1, &ibo);

  vertexBuffer = {{
                      {-0.5f, 0.5f},        // position
                      {1.f, 0.f, 0.f, 1.f}  // color
                  },
                  {{0.5f, 0.5f}, {1.f, 0.5f, 0.f, 1.f}},
                  {{0.5f, -0.5f}, {1.f, 1.f, 0.f, 1.f}},
                  {{-0.5f, -0.5f}, {1.f, 0.f, 1.f, 1.f}}};
  indexBuffer = {
    0, 1, 2,
    0, 2, 3
  };

    // Bind the VBO
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //send the vertices data.
  openGLContext.extensions.glBufferData(GL_ARRAY_BUFFER,
                                        sizeof(Vertex) * vertexBuffer.size(),
                                        vertexBuffer.data(), GL_STATIC_DRAW);
                                        //Bind the IBO
  openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  // Send the indices data.
  openGLContext.extensions.glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    sizeof(unsigned int) * indexBuffer.size(),
    indexBuffer.data(),
    GL_STATIC_DRAW
  );


  vertexShader = R"(
 attribute vec3 inPosition;
attribute vec3 inColor;
uniform vec3 positionOffset;
varying vec3 fragColor;
void main()
    {
        vec3 modifiedPosition = inPosition + positionOffset;
        fragColor = inColor;
        gl_Position = vec4(modifiedPosition, 1.0);
    }

        )";

  fragmentShader =
      R"(
// Fragment Shader
varying vec3 fragColor;  // Interpolated color from the vertex shader

void main()
{
    // Output the interpolated color
    gl_FragColor = vec4(fragColor, 1.0);  // Alpha is set to 1.0 (fully opaque)
    gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);

    if (gl_FragCoord.x/400.0 > 1.0){
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    if (mod(gl_FragCoord.x, 16.0)<=8.0){
    gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);
    }
    }
//     if (gl_FragCoord.x > 100.0){
//         int i_coord = int(gl_FragCoord.x);
//         if ( mod(gl_FragCoord.x, 2.0) == 0.0){
// }
//     }

}
        )";

  shaderProgram.reset(new OpenGLShaderProgram(openGLContext));

  if (shaderProgram->addVertexShader(juce::OpenGLHelpers::translateVertexShaderToV3(vertexShader)) &&
      shaderProgram->addFragmentShader(juce::OpenGLHelpers::translateFragmentShaderToV3(fragmentShader)) &&
      shaderProgram->link()) {
    shaderProgram->use();
} else {
  jassertfalse;
}

        // Retrieve the location of the "positionOffset" uniform from the shader program
        positionOffsetLocation = openGLContext.extensions.glGetUniformLocation(shaderProgram->getProgramID(), "positionOffset");

        // Initialize positionOffset to 0 (no offset at first)
        positionOffset[0] = 0.0f;
        positionOffset[1] = 0.0f;
        positionOffset[2] = 0.0f;
}
void OpenGLComponent::renderOpenGL() { OpenGLHelpers::clear(Colours::black);
  shaderProgram->use();
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vbo);
  openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
      // ...
    
    // Enable the position attribute.
    openGLContext.extensions.glVertexAttribPointer(
        0,              // The attribute's index (AKA location).
        2,              // How many values this attribute contains.
        GL_FLOAT,       // The attribute's type (float).
        GL_FALSE,       // Tells OpenGL NOT to normalise the values.
        sizeof(Vertex), // How many bytes to move to find the attribute with
                        // the same index in the next vertex.
        nullptr         // How many bytes to move from the start of this vertex
                        // to find this attribute (the default is 0 so we just
                        // pass nullptr here).
    );
    openGLContext.extensions.glEnableVertexAttribArray(0);
    
    // Enable to colour attribute.
    openGLContext.extensions.glVertexAttribPointer(
        1,                              // This attribute has an index of 1
        4,                              // This time we have four values for the
                                        // attribute (r, g, b, a)
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (GLvoid*)(sizeof(float) * 2)    // This attribute comes after the
                                        // position attribute in the Vertex
                                        // struct, so we need to skip over the
                                        // size of the position array to find
                                        // the start of this attribute.
    );
    openGLContext.extensions.glEnableVertexAttribArray(1);

    openGLContext.extensions.glUniform3f(positionOffsetLocation, positionOffset[0], positionOffset[1], positionOffset[2]);


        // ...
    glDrawElements(
        GL_TRIANGLES,       // Tell OpenGL to render triangles.
        indexBuffer.size(), // How many indices we have.
        GL_UNSIGNED_INT,    // What type our indices are.
        nullptr             // We already gave OpenGL our indices so we don't
                            // need to pass that again here, so pass nullptr.
    );

        openGLContext.extensions.glDisableVertexAttribArray(0);
    openGLContext.extensions.glDisableVertexAttribArray(1);
}
void OpenGLComponent::openGLContextClosing() {}

void OpenGLComponent::resized(){        
    transparentOverlay.setBounds(getLocalBounds());  // Make sure overlay covers the entire window
}

void OpenGLComponent::paint(Graphics& g) {


}