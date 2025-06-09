import os
from pathlib import Path
import cairosvg

def main():
    # Set up paths
    base_dir = Path(__file__).parent.parent
    svg_dir = base_dir / 'docs' / 'assets' / 'branding' / 'svg'
    png_dir = base_dir / 'docs' / 'assets' / 'branding' / 'png'
    
    # Create output directory if it doesn't exist
    png_dir.mkdir(parents=True, exist_ok=True)
    
    # Standard sizes (width in pixels)
    sizes = [16, 32, 64, 128, 256, 512, 1024, 2048]  # 2048 for high-DPI/print
    
    # Process each SVG file
    for svg_file in svg_dir.glob('*.svg'):
        base_name = svg_file.stem
        
        for size in sizes:
            output_file = png_dir / f"{base_name}-{size}x{size}.png"
            print(f"Generating {output_file.relative_to(base_dir)}...")
            
            try:
                cairosvg.svg2png(
                    url=str(svg_file),
                    write_to=str(output_file),
                    output_width=size,
                    output_height=size
                )
            except Exception as e:
                print(f"  Error: {e}")
    
    print("\nPNG generation complete!")
    print(f"Generated files are in: {png_dir.relative_to(base_dir)}")

if __name__ == "__main__":
    main()
