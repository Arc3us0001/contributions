// Learning Objective: This tutorial demonstrates how to generate and visualize the Mandelbrot set fractal using C++ and a simple graphics library.
// We will learn about:
// 1. The Mandelbrot set definition and its iterative nature.
// 2. Mapping complex numbers to screen coordinates.
// 3. Iterating a complex function to determine if a point belongs to the set.
// 4. Using a basic graphics library (like SFML) to draw pixels and visualize the result.
// This example assumes you have SFML installed. For installation instructions, visit https://www.sfml-dev.org/download.php

#include <SFML/Graphics.hpp> // Include the SFML graphics library for window and drawing
#include <complex>           // Include the complex number library for complex arithmetic

// Define the dimensions of our output window
const int WIDTH = 800;
const int HEIGHT = 800;

// Define the region of the complex plane we want to visualize
// This is the standard view of the Mandelbrot set
const double RE_START = -2.0;
const double RE_END = 1.0;
const double IM_START = -1.5;
const double IM_END = 1.5;

// Maximum number of iterations to determine if a point is in the set
// Higher values result in more detail but take longer to compute
const int MAX_ITERATIONS = 100;

// Function to calculate the Mandelbrot iteration for a given complex number
// It returns the number of iterations before the magnitude exceeds 2 (diverges)
// or MAX_ITERATIONS if it stays bounded.
int mandelbrot(std::complex<double> c) {
    std::complex<double> z = 0; // Initialize z to 0 for each point
    int iterations = 0;         // Initialize iteration count

    // The core of the Mandelbrot algorithm: z = z*z + c
    // We repeat this process up to MAX_ITERATIONS times.
    while (std::abs(z) < 2.0 && iterations < MAX_ITERATIONS) {
        z = z * z + c;      // This is the Mandelbrot iteration formula
        iterations++;       // Increment the iteration counter
    }
    return iterations;      // Return the number of iterations
}

int main() {
    // Create an SFML window
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Mandelbrot Set");

    // Create an SFML Image object to store the pixel data
    sf::Image image;
    image.create(WIDTH, HEIGHT); // Create the image with our defined dimensions

    // Iterate over each pixel in the window
    for (int x = 0; x < WIDTH; ++x) {
        for (int y = 0; y < HEIGHT; ++y) {
            // Map the pixel coordinates (x, y) to a point in the complex plane
            // This is a linear transformation:
            // real part: scales x from [0, WIDTH] to [RE_START, RE_END]
            double re = RE_START + (double)x / WIDTH * (RE_END - RE_START);
            // imaginary part: scales y from [0, HEIGHT] to [IM_START, IM_END]
            // We invert y because screen coordinates usually have (0,0) at the top-left,
            // while the complex plane has the imaginary axis increasing upwards.
            double im = IM_START + (double)y / HEIGHT * (IM_END - IM_START);

            // Create a complex number from the mapped real and imaginary parts
            std::complex<double> c(re, im);

            // Calculate the number of iterations for this complex number
            int iterations = mandelbrot(c);

            // Determine the color of the pixel based on the iteration count
            // If iterations == MAX_ITERATIONS, the point is likely in the Mandelbrot set (black)
            // Otherwise, we use the iteration count to create a gradient for the points outside the set.
            sf::Color color;
            if (iterations == MAX_ITERATIONS) {
                color = sf::Color::Black; // Points inside the set are black
            } else {
                // Create a simple color gradient based on the number of iterations
                // We'll use shades of blue and green.
                // The modulo operator (%) helps to cycle through colors if MAX_ITERATIONS is large
                // and prevents a single color from dominating.
                int r = (iterations * 5) % 255;
                int g = (iterations * 10) % 255;
                int b = (iterations * 15) % 255;
                color = sf::Color(r, g, b);
            }

            // Set the pixel color in the image
            image.setPixel(x, y, color);
        }
    }

    // Create an SFML Texture from the Image
    sf::Texture texture;
    texture.loadFromImage(image);

    // Create an SFML Sprite to display the Texture
    sf::Sprite sprite;
    sprite.setTexture(texture);

    // Main application loop
    while (window.isOpen()) {
        // Process events (like closing the window)
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Clear the window
        window.clear();

        // Draw the sprite (which contains our Mandelbrot set visualization)
        window.draw(sprite);

        // Display the contents of the window
        window.display();
    }

    return 0; // Indicate successful execution
}

// Example Usage:
// 1. Compile this code with an SFML setup. For example, using g++:
//    g++ -o mandelbrot mandelbrot.cpp -lsfml-graphics -lsfml-window -lsfml-system
// 2. Run the executable:
//    ./mandelbrot
//
// A window will open displaying the Mandelbrot fractal.
// You can experiment by changing RE_START, RE_END, IM_START, IM_END, and MAX_ITERATIONS
// to zoom into different parts of the fractal or increase detail.