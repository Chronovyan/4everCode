// 3D Logo Effect
document.addEventListener('DOMContentLoaded', function() {
    // Create 3D logo layers
    function createLogoLayers(container, count = 3) {
        if (!container) return;
        
        // Clear any existing layers
        container.innerHTML = '';
        
        // Create layers
        for (let i = 0; i < count; i++) {
            const layer = document.createElement('div');
            layer.className = 'mirror-layer absolute flex items-center justify-center';
            layer.textContent = '.4e';
            container.appendChild(layer);
        }
    }
    
    // Initialize logos
    const logos = document.querySelectorAll('.hall-of-mirrors-container');
    logos.forEach(logo => {
        createLogoLayers(logo, 3);
    });
    
    // Mobile menu toggle
    const mobileMenuButton = document.querySelector('[data-mobile-menu-button]');
    const mobileMenu = document.querySelector('[data-mobile-menu]');
    
    if (mobileMenuButton && mobileMenu) {
        mobileMenuButton.addEventListener('click', () => {
            mobileMenu.classList.toggle('hidden');
        });
    }
});

// Smooth scrolling for anchor links
document.querySelectorAll('a[href^="#"]').forEach(anchor => {
    anchor.addEventListener('click', function (e) {
        e.preventDefault();
        const target = document.querySelector(this.getAttribute('href'));
        if (target) {
            target.scrollIntoView({
                behavior: 'smooth',
                block: 'start'
            });
        }
    });
});
