/** @type {import('next').NextConfig} */
const nextConfig = {
  reactStrictMode: true,
  distDir: process.env.NODE_ENV === "development" ? ".next-dev" : ".next",
  // Native C++ requests need a Node server. Separate caches keep dev/build independent.
};

export default nextConfig;
