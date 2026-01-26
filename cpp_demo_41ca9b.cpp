// //////////////////////////////////////////////////////////////////////////////
// /////////////////// REAL-TIME AUDIO VISUALIZER TUTORIAL ////////////////////
// //////////////////////////////////////////////////////////////////////////////
// //                                                                          //
// // This tutorial demonstrates how to create a basic real-time audio        //
// // visualizer in C++ using the SFML library. The primary learning objective //
// // is to understand the fundamental concepts of:                           //
// //                                                                          //
// // 1. **Audio Playback:** Loading and playing an audio file.                //
// // 2. **Audio Data Retrieval:** Accessing raw audio sample data.           //
// // 3. **Basic Audio Processing:** Understanding amplitude and frequency.    //
// // 4. **Graphics Rendering:** Drawing visual representations based on audio. //
// //                                                                          //
// // We will focus on visualizing the *amplitude* (loudness) of the audio     //
// // at different points in time to create a simple waveform.                  //
// //                                                                          //
// // Prerequisites:                                                           //
// // - Basic C++ knowledge.                                                   //
// // - SFML library installed and configured for your compiler.               //
// //   (See SFML's official website for installation instructions.)            //
// //                                                                          //
// // //////////////////////////////////////////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cmath> // For std::abs and std::sin/cos if we wanted frequency later

// Define some constants for our window and visualization
const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;
const int MAX_SAMPLES_TO_DISPLAY = 500; // How many audio samples we'll process at once
const float AMPLITUDE_SCALE = 50.0f;  // How much to stretch the waveform vertically

int main() {
    // 1. Set up the SFML Window
    // This creates our graphical window where the visualization will be displayed.
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML Audio Visualizer");
    window.setFramerateLimit(60); // Limit frame rate for smoother rendering

    // 2. Load the Audio File
    // We need an audio file to visualize. SFML supports various formats like WAV, OGG, etc.
    // Make sure 'your_audio_file.ogg' is in the same directory as your executable
    // or provide the correct path.
    sf::Music music;
    if (!music.openFromFile("your_audio_file.ogg")) { // Replace with your audio file
        // If the file cannot be opened, print an error and exit.
        // This is crucial for debugging.
        return -1; // Indicate an error
    }

    // 3. Prepare for Audio Data Retrieval
    // SFML's sf::Music provides a way to get the raw audio samples.
    // We'll use this to determine the amplitude.
    // getSampleCount() tells us the total number of samples in the audio.
    // getSampleRate() tells us how many samples are played per second.
    // getChannelCount() tells us if it's mono (1) or stereo (2). We'll use the first channel.
    const sf::Int16* samples = music.getSamples(); // Get a pointer to the raw audio data
    sf::Uint32 numSamples = music.getSampleCount();
    unsigned int sampleRate = music.getSampleRate();

    // If the audio is stereo, we only need one channel for a simple visualizer.
    // We'll use the first channel (index 0).
    unsigned int channels = music.getChannelCount();

    // 4. Prepare for Graphics Rendering
    // We'll use sf::VertexArray to draw lines representing the waveform.
    // sf::Lines means we draw pairs of vertices as individual lines.
    sf::VertexArray waveform(sf::Lines, MAX_SAMPLES_TO_DISPLAY * 2);

    // 5. The Main Application Loop
    // This loop continues as long as the window is open.
    while (window.isOpen()) {
        // 6. Event Handling
        // We process events like closing the window or pressing keys.
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close(); // Close the window if the user clicks the close button.
            }
            // Basic control: Spacebar to play/pause
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    if (music.getStatus() == sf::Music::Playing) {
                        music.pause(); // Pause the music if it's playing.
                    } else {
                        music.play();  // Play the music if it's paused or stopped.
                    }
                }
            }
        }

        // 7. Audio Processing and Visualization Logic
        // We need to get the current audio position and extract a chunk of samples
        // to visualize.
        sf::Time currentTime = music.getPlayingOffset();
        sf::Uint32 currentSampleIndex = static_cast<sf::Uint32>(currentTime.asSeconds() * sampleRate);

        // Ensure we don't go out of bounds
        if (currentSampleIndex >= numSamples) {
            currentSampleIndex = numSamples - 1; // Clamp to the last sample
        }

        // Iterate through a limited number of samples to draw.
        // This creates a scrolling effect.
        for (int i = 0; i < MAX_SAMPLES_TO_DISPLAY; ++i) {
            sf::Uint32 sampleIndex = currentSampleIndex + i;

            // Check if we've reached the end of the audio
            if (sampleIndex >= numSamples) {
                break; // Stop drawing if we're past the end
            }

            // Get the amplitude of the current sample.
            // sf::Int16 samples are typically in the range of -32768 to +32767.
            // We use the first channel if stereo.
            float sampleValue = static_cast<float>(samples[sampleIndex * channels]);

            // Normalize the sample value to a range between -1 and 1.
            // Dividing by 32768 (max positive value for Int16) is a common way.
            float normalizedAmplitude = sampleValue / 32768.0f;

            // Calculate the y-coordinate for the waveform.
            // The center of the screen is WINDOW_HEIGHT / 2.
            // We multiply by AMPLITUDE_SCALE to make the waveform visible.
            float yPos = WINDOW_HEIGHT / 2.0f - normalizedAmplitude * AMPLITUDE_SCALE;

            // Define the x-coordinate for the current sample.
            // This creates the horizontal progression of the waveform.
            float xPos = static_cast<float>(i) * (static_cast<float>(WINDOW_WIDTH) / MAX_SAMPLES_TO_DISPLAY);

            // Set the vertices for the current line segment.
            // Each line segment is defined by two points: a start and an end.
            // For a waveform, we often draw vertical lines to represent amplitude at a point.
            // Here, we're drawing a line from the center to the calculated yPos.

            // Vertex 1: The starting point of the line (on the center line)
            waveform[i * 2].position = sf::Vector2f(xPos, WINDOW_HEIGHT / 2.0f);
            waveform[i * 2].color = sf::Color::Green; // Set the color of the line

            // Vertex 2: The ending point of the line (at the calculated amplitude)
            waveform[i * 2 + 1].position = sf::Vector2f(xPos, yPos);
            waveform[i * 2 + 1].color = sf::Color::Cyan; // Set the color of the line
        }

        // 8. Drawing
        window.clear(sf::Color::Black); // Clear the window with a black background.

        // Draw the waveform.
        // This function renders all the vertices we've defined in the 'waveform' array.
        window.draw(waveform);

        window.display(); // Update the window to show what we've drawn.
    }

    return 0; // Indicate successful execution.
}

// //////////////////////////////////////////////////////////////////////////////
// // Example Usage:                                                           //
// //                                                                          //
// // 1. Save this code as a .cpp file (e.g., visualizer.cpp).                 //
// // 2. Make sure you have an audio file (e.g., 'your_audio_file.ogg') in    //
// //    the same directory. Replace "your_audio_file.ogg" in the code with   //
// //    your actual audio file name.                                         //
// // 3. Compile the code using a C++ compiler that has SFML configured.       //
// //    Example using g++ (Linux/macOS):                                     //
// //    g++ visualizer.cpp -o visualizer -lsfml-graphics -lsfml-window -lsfml-audio //
// //    Example using MSVC (Windows):                                         //
// //    (You'll need to set up SFML project properties in Visual Studio)      //
// // 4. Run the executable:                                                   //
// //    ./visualizer (Linux/macOS)                                           //
// //    visualizer.exe (Windows)                                             //
// //                                                                          //
// // You should see a window with a scrolling waveform. Press SPACE to play/pause. //
// //                                                                          //
// // //////////////////////////////////////////////////////////////////////////