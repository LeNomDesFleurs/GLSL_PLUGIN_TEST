
#include <JuceHeader.h>

#include <string>
class OpenGLComponent : public Component, public OpenGLRenderer {
  public:
  OpenGLComponent();
  ~OpenGLComponent();

  void newOpenGLContextCreated() override;
  void renderOpenGL() override;
  void openGLContextClosing() override;
  void paint(Graphics &g) override;
  void resized() override;

 private:
  OpenGLContext openGLContext;

  std::unique_ptr<OpenGLShaderProgram> shaderProgram;
  String vertexShader;
  String fragmentShader;

  struct Vertex {
    float position[2];
    float colour[4];
  };

  GLuint vbo;
  GLuint ibo;
  GLuint positionOffsetLocation;
  GLfloat positionOffset[3];

  std::vector<Vertex> vertexBuffer;
  std::vector<unsigned int> indexBuffer;

  juce::Slider slider;

  class TransparentInteractionOverlay : public juce::Component
{
public:
    std::function<void(float, float)> onMouseDragCallback; // Callback to update the uniform

    TransparentInteractionOverlay()
    {
        setInterceptsMouseClicks(true, true);  // Allow this component to intercept mouse events
        setOpaque(false);  // Make the component transparent
    }

    // Override the paint method to ensure transparency
    void paint(Graphics& g) override
    {
        // No need to paint anything, the component is transparent
    }

    // Handle mouse dragging
    void mouseDrag(const juce::MouseEvent& event) override
    {
        if (onMouseDragCallback != nullptr)
        {
            // Convert the mouse position to a normalized range (-1, 1)
            float normalizedX = (float)event.getPosition().getX() / (float)getWidth() * 2.0f - 1.0f;
            float normalizedY = -((float)event.getPosition().getY() / (float)getHeight() * 2.0f - 1.0f);

            // Call the callback to update the position offset
            onMouseDragCallback(normalizedX, normalizedY);
        }
    }
};

    TransparentInteractionOverlay transparentOverlay;

    // This function updates the position offset based on user input and triggers a redraw
    void updatePositionOffset(float x, float y)
    {
        positionOffset[0] = x;  // Update X offset
        positionOffset[1] = y;  // Update Y offset
        positionOffset[2] = 0.0f;  // Z remains unchanged

        // Trigger re-rendering
        repaint();
    }

};