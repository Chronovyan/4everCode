const puppeteer = require('puppeteer');
const path = require('path');
const fs = require('fs');

// Ensure output directory exists
const outputDir = path.join(__dirname, '../../assets/images/social');
if (!fs.existsSync(outputDir)) {
    fs.mkdirSync(outputDir, { recursive: true });
}

(async () => {
    // Launch Puppeteer
    const browser = await puppeteer.launch({
        headless: 'new',
        args: ['--no-sandbox', '--disable-setuid-sandbox']
    });
    
    try {
        const page = await browser.newPage();
        
        // Set viewport size for social media (1200x630 is standard for Open Graph)
        await page.setViewport({
            width: 1200,
            height: 630,
            deviceScaleFactor: 2 // For better quality
        });
        
        // Get the absolute path to the HTML file
        const htmlPath = path.join(__dirname, '../../assets/images/social/core-concepts-preview.html');
        const fileUrl = `file://${htmlPath.replace(/\\/g, '/')}`;
        
        // Navigate to the HTML file
        await page.goto(fileUrl, { waitUntil: 'networkidle0' });
        
        // Wait for fonts to load
        await page.evaluateHandle('document.fonts.ready');
        
        // Take a screenshot
        const outputPath = path.join(outputDir, 'core-concepts-social.png');
        await page.screenshot({
            path: outputPath,
            type: 'png',
            fullPage: false
        });
        
        console.log(`Social preview image generated at: ${outputPath}`);
    } catch (error) {
        console.error('Error generating social preview image:', error);
        process.exit(1);
    } finally {
        await browser.close();
    }
})();
