// Add smooth scrolling for anchor links
document.addEventListener('DOMContentLoaded', function() {
    // Add smooth scrolling to all anchor links
    document.querySelectorAll('a[href^="#"]').forEach(anchor => {
        anchor.addEventListener('click', function (e) {
            e.preventDefault();
            
            const targetId = this.getAttribute('href');
            if (targetId === '#') return;
            
            const targetElement = document.querySelector(targetId);
            if (targetElement) {
                targetElement.scrollIntoView({
                    behavior: 'smooth',
                    block: 'start'
                });
            }
        });
    });

    // Add copy button to code blocks
    document.querySelectorAll('.highlight').forEach(block => {
        // Create copy button
        const button = document.createElement('button');
        button.className = 'md-clipboard';
        button.title = 'Copy to clipboard';
        
        // Add icon
        const icon = document.createElement('span');
        icon.className = 'md-icon';
        icon.textContent = 'content_copy';
        button.appendChild(icon);
        
        // Add success message
        const success = document.createElement('span';
        success.className = 'md-clipboard--success';
        success.textContent = 'Copied!';
        button.appendChild(success);
        
        // Add button to code block
        block.style.position = 'relative';
        block.insertBefore(button, block.firstChild);
        
        // Add click event
        button.addEventListener('click', function() {
            const code = block.querySelector('code').textContent;
            navigator.clipboard.writeText(code).then(() => {
                // Show success message
                const success = this.querySelector('.md-clipboard--success');
                success.style.opacity = '1';
                setTimeout(() => {
                    success.style.opacity = '0';
                }, 2000);
            }).catch(err => {
                console.error('Failed to copy text: ', err);
            });
        });
    });
});

// Add animation to feature cards on scroll
const observerOptions = {
    threshold: 0.1,
    rootMargin: '0px 0px -50px 0px'
};

const observer = new IntersectionObserver((entries, observer) => {
    entries.forEach(entry => {
        if (entry.isIntersecting) {
            entry.target.style.opacity = '1';
            entry.target.style.transform = 'translateY(0)';
            observer.unobserve(entry.target);
        }
    });
}, observerOptions);

document.querySelectorAll('.grid.cards > div').forEach((card, index) => {
    card.style.opacity = '0';
    card.style.transform = 'translateY(20px)';
    card.style.transition = `opacity 0.5s ease ${index * 0.1}s, transform 0.5s ease ${index * 0.1}s`;
    observer.observe(card);
});
