#pragma once

#include <vector>

struct Color {
	float r, g, b;

	Color();
	Color(float r, float g, float b);
	~Color();
};

class Image
{
	public:
		Image(int width, int height);
    	~Image();

		// Get pixel color
		Color GetColor(int x, int y) const;

		// Set pixel color
		void SetColor(const Color& color, int x, int y);

		// Set pixel colors for a region
		void SetColorPartial(const Color& color, int x1, int y1, int x2, int y2);

		// Set pixel colors for a region
		void ChangeColor(int x1, int y1, int x2, int y2);

		// Read image file from disk
		void Read(const char* path);

		// Write image file from memory
		void Export(const char* path) const;
	private:
		int m_width;
		int m_height;
		std::vector<Color> m_colors;
};
