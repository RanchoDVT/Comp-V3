const fs = require('fs');
const path = require('path');
const { minify } = require('terser');
const csso = require('csso');
const zlib = require('zlib');

// Function to minify JavaScript
async function minifyJS(inputPath, outputPath) {
    const code = fs.readFileSync(inputPath, 'utf8');
    const result = await minify(code, {
        compress: {
            drop_console: false, // Keep console logs for debugging
            passes: 2
        },
        mangle: true
    });

    fs.writeFileSync(outputPath, result.code);
    console.log(`Minified JS: ${inputPath} → ${outputPath}`);

    // Create gzipped version
    const gzipped = zlib.gzipSync(result.code);
    fs.writeFileSync(`${outputPath}.gz`, gzipped);
    console.log(`Compressed JS: ${outputPath} → ${outputPath}.gz (${Math.round(gzipped.length / result.code.length * 100)}% of original)`);
}

// Function to minify CSS
function minifyCSS(inputPath, outputPath) {
    const css = fs.readFileSync(inputPath, 'utf8');
    const result = csso.minify(css).css;

    fs.writeFileSync(outputPath, result);
    console.log(`Minified CSS: ${inputPath} → ${outputPath}`);

    // Create gzipped version
    const gzipped = zlib.gzipSync(result);
    fs.writeFileSync(`${outputPath}.gz`, gzipped);
    console.log(`Compressed CSS: ${outputPath} → ${outputPath}.gz (${Math.round(gzipped.length / result.length * 100)}% of original)`);
}

// Process files
(async () => {
    // Create dist directory
    const distDir = path.join(__dirname, 'dist');
    if (!fs.existsSync(distDir)) {
        fs.mkdirSync(distDir);
    }

    // Minify JS
    await minifyJS(
        path.join(__dirname, 'main.js'),
        path.join(distDir, 'main.min.js')
    );

    // Minify CSS
    minifyCSS(
        path.join(__dirname, 'style.css'),
        path.join(distDir, 'style.min.css')
    );

    console.log('Build completed successfully!');
})();