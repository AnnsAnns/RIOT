/** @type {import('tailwindcss').Config} */
module.exports = {
  content: ["./header.html", "./footer.html"],
  theme: {
    extend: {
      colors: {
        "riot-green": "#40A687",
        "riot-red": "#BC202A",
      }
    },
  },
  plugins: [],
}
