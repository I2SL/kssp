#include "../libkessler/stage.h"

double get_phi(int x, int nx, double hfov) {
    return atan(2 * x * tan(hfov) / nx);
}

double get_theta(int y, int ny, double hfov) {
    return atan((float)ny / 2 / (float)y / tan(hfov));
}

double get_phi_prime(double phi, double theta, double y0, double r) {
    return phi - (y0 / r) * (cos(phi) / sin(theta));
}

double get_theta_prime(double phi, double theta, double y0, double r) {
    return theta - (y0 / r) * cos(theta) * sin(phi);
}

int main()
{
    double hfovx;
    double hfovy;
    double px;
    double f;
    double y0;
    double r;
    int ny;
    int nx;
    int y;
    int x;
    printf("Enter target distance (or 0 to quit):\n");
    std::cin >> r;
    printf("Enter focal length in millimeters:\n");
    std::cin >> f;
    f *= pow(10, -3);
    printf("Enter sensor separation in meters:\n");
    std::cin >> y0;
    printf("Enter number of vertical pixels:\n");
    std::cin >> ny;
    printf("Enter number of horizontal pixels:\n");
    std::cin >> nx;
    printf("Enter pixel size in microns:\n");
    std::cin >> px;
    px *= pow(10, -6);

    hfovx = atan(px * nx / f / 2);
    hfovy = atan(px * ny / f / 2);

    printf("Enter target x coordinate:\n");
    std::cin >> x;
    printf("Enter target y coordinate:\n");
    std::cin >> y;

    double theta = get_theta(y, ny, hfovy);
    double phi = get_phi(x, nx, hfovx);
    double theta_prime = get_theta_prime(phi, theta, y0, r);
    double phi_prime = get_phi_prime(phi, theta, y0, r);

    printf("Calculated Coordinates in degrees (theta, phi):\n");
    printf("EBS: (%.2f, %.2f)\n", theta * 180 / M_PI, phi * 180 / M_PI);
    printf("FBC: (%.2f, %.2f)\n", theta_prime * 180 / M_PI, phi_prime * 180 / M_PI);
    printf("\n");
}