#ifndef ZBUFFER_H
#define ZBUFFER_H

class ZBuffer
{
private:
    Utility util;
    fstream fin_config, fin_input, fout_buffer;
    int width, height;
    double ZMIN = -1.0, ZMAX = 1.0, x, y, z;
    vector<Triangle> triangles;
    double rightLimitX = 1.0, leftLimitX = -1.0, topLimitY = 1.0, bottomLimitY = -1.0; // Volume Box
    double **zBuffer;

    void initZBuffer(int height, int width)
    {
        zBuffer = new double *[width];
        for (int i = 0; i < width; i++)
        {
            zBuffer[i] = new double[height];
        }
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {
                zBuffer[i][j] = ZMAX;
            }
        }
    }

    void clearZBuffer()
    {
        // clear the z-buffer
        for (int i = 0; i < width; i++)
        {
            delete[] zBuffer[i];
        }
    }

    void writeZBufferToFile()
    {
        // Write the z-buffer to "z_buffer.txt"
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                if (zBuffer[j][i] < ZMAX)
                    fout_buffer << fixed << setprecision(6) << zBuffer[j][i] << "\t";
            }
            fout_buffer << endl;
        }
    }

    double intersect(double x1, double y1, double x2, double y2, double y)
    {
        double m = (y2 - y1) / (x2 - x1);
        double x = x1 + (y - y1) / m;
        return x;
    }

    void adjust(double &za, double &zb, double prev_xa, double prev_xb, double xa, double xb)
    {
        if (xa != prev_xa)
        {
            za = intersect(za, prev_xa, zb, prev_xb, xa);
        }
        if (xb != prev_xb)
        {
            zb = intersect(za, prev_xa, zb, prev_xb, xb);
        }
    }

public:
    ZBuffer(string config_file, string input_file, string output_image, string output_zbuffer)
    {
        fin_config.open(config_file, ios::in);
        fin_input.open(input_file, ios::in);
        fout_buffer.open(output_zbuffer, ios::out);

        fin_config >> width >> height;
    }

    void calculateZBuffer()
    {
        // Read triangle objects from "stage3.txt"
        bool eof_file = false;
        while (true)
        {
            vector<PT> points;
            // Per 3 lines denote a triangle
            for (int i = 0; i < 3; i++)
            {
                if (fin_input >> x >> y >> z)
                    points.push_back(PT(x, y, z));
                else
                {
                    eof_file = true;
                    break;
                }
            }
            if (eof_file)
                break;
            int red_channel = util.random();
            int green_channel = util.random();
            int blue_channel = util.random();

            triangles.push_back(Triangle(points[0], points[1], points[2], red_channel, green_channel, blue_channel));
        }
        // Checking if the read is correct
        // for (Triangle t: triangles) {
        //     cout << t << endl;
        // }

        // Image bounding box limits
        double dx = (double)(rightLimitX - leftLimitX) / width;
        double dy = (double)(topLimitY - bottomLimitY) / height;
        cout << "dx: " << dx << " dy: " << dy << endl;

        // Middle points of the pixels (screen's cell centers)
        double left_x = leftLimitX + dx / 2.0;
        double right_x = rightLimitX - dx / 2.0;
        double top_y = topLimitY - dy / 2.0;
        double bottom_y = bottomLimitY + dy / 2.0;

        // During scanning from top to bottom and left to right, check for the middle values of each cell

        // Initialize the z-buffer
        initZBuffer(height, width); // height == #rows and width == #columns

        // Create a bitmap image with the given width and height
        bitmap_image image(width, height);
        // Set the background color to black
        image.set_all_channels(0);
        std::ofstream print("out.txt");
        for (Triangle triangle : triangles)
        {
            double minx = INT_MAX, miny = INT_MAX, maxx = INT_MIN, maxy = INT_MIN;
            // Find the min and max of x and y coordinates of the triangle's corners
            for (PT p : triangle.getPoints())
            {
                minx = min(minx, p.x);
                maxx = max(maxx, p.x);
                miny = min(miny, p.y);
                maxy = max(maxy, p.y);
            }
            // cout << triangle << endl;

            // Clipping
            minx = max(minx, left_x);
            maxx = min(maxx, right_x);
            miny = max(miny, bottom_y);
            maxy = min(maxy, top_y);

            // Find the maxY scanline and minY scanline
            int maxYScanline = round((top_y - maxy) / dy);
            int minYScanline = round((top_y - miny) / dy);
            // Why divide by dy? World Space to Raster Space (0, 1, 2...)

            // Scan from top to bottom (along Y axis)
            for (int scanning_y = maxYScanline; scanning_y <= minYScanline; scanning_y++)
            {
                double scanlineY = top_y - scanning_y * dy; // In world space

                // find the intersection points of the scanline with the triangle, where the scanline may intersect any two lines of the triangle
                // For each edge of the triangle
                vector<pair<double, double>> intersections;
                PT p1, p2;
                // For each edge of the triangle
                for (int i = 0; i < 3; i++)
                {
                    p1 = triangle.getPoints()[i];
                    p2 = triangle.getPoints()[(i + 1) % 3];

                    // Check if the edge is horizontal
                    // Jodi horizontal hoy, division by zero error may occur, condition check if (1st condn)

                    // Intersection point 2 ta hobe at max except if the scanline intersects a top point of the triangle
                    // Jodi scanlineY duita point er line er majhe diye jay, tahole oder Y range er majhe thakbe
                    // z-cor ta ber korte hobe, triangle ta y/x axis or any other vector er respect e rotated thakte pare
                    if ((p1.y != p2.y) and (scanlineY >= min(p1.y, p2.y)) and (scanlineY <= max(p1.y, p2.y)))
                    {
                        x = intersect(p1.x, p1.y, p2.x, p2.y, scanlineY);
                        z = intersect(p1.z, p1.y, p2.z, p2.y, scanlineY);
                        print<< x << " " << z << endl;

                        intersections.push_back({x, z});
                    }
                }
                if (intersections.size() == 0)
                    continue; // due to precision loss (-.002 not 0)

                // Sort the intersection points by x-coordinate
                std::sort(intersections.begin(), intersections.end(), [](const auto &a, const auto &b)
                          { return a.first < b.first; });

                // Case: Hote pare je triangle er x-cor out of the volume box, tai jodi oita intersecting point hoye thake ba choto ba boro hoy then x-clipping ou korte hobe
                double prev_xa = intersections[0].first, prev_xb = intersections[1].first;
                double prev_za = intersections[0].second, prev_zb = intersections[1].second;

                // Case: Jodi x-cor clip kora hoy, z-cor ta change hoyeche, tai z-cor update korte hobe
                // Etar jonno previous x-cor ta lagbe, regression formulae ta use korte hole

                double xa = intersections[0].first, xb = intersections[1].first, za = intersections[0].second, zb = intersections[1].second;

                if (xa < minx)
                    xa = minx;
                if (xa > maxx)
                    xa = maxx;
                if (xb < minx)
                    xb = minx;
                if (xb > maxx)
                    xb = maxx;

                // Case: Jodi x-cor clip kora hoy, z-cor ta change hoyeche, tai z-cor update korte hobe
                adjust(za, zb, prev_xa, prev_xb, xa, xb);
                // za = zb - (zb - za) * (prev_xb - xa) / (prev_xb - prev_xa);
                // zb = zb - (zb - za) * (prev_xb - xb) / (prev_xb - prev_xa);

                double minXScanline = round((xa - left_x) / dx);
                double maxXScanline = round((xb - left_x) / dx);

                // Scan from left to right (along X axis)
                for (int scanning_x = minXScanline; scanning_x <= maxXScanline; scanning_x++)
                {
                    double scanlineX = left_x + scanning_x * dx; // In world space

                    // we know the (x, y) of the pixel, we need to find the z
                    z = intersect(za, xa, zb, xb, scanlineX);
                    // double z = zb - (zb - za) * (xb - scanlineX) / (xb - xa);
                    // Case: Jodi z-buffer er value boro hoy, tahole oita update korte hobe
                    // z also has to be greater than z_front_limit
                    if (z < zBuffer[scanning_x][scanning_y] and z >= ZMIN)
                    {
                        zBuffer[scanning_x][scanning_y] = z;

                        // Case: Jodi z-buffer er value boro hoy, tahole oita update korte hobe
                        // Color update korte hobe
                        int red_channel = triangle.getColors()[0];
                        int green_channel = triangle.getColors()[1];
                        int blue_channel = triangle.getColors()[2];

                        // Image being the frame buffer
                        image.set_pixel(scanning_x, scanning_y, red_channel, green_channel, blue_channel);
                    }
                }
            }
        }

        writeZBufferToFile();

        image.save_image("out.bmp"); // Save the image to "out.bmp"

        // closing the file reader objects
        fin_config.close();
        fin_input.close();
        fout_buffer.close();

        clearZBuffer();
        // clear image object
        image.clear();
        image.setwidth_height(0, 0);
    }
};

#endif