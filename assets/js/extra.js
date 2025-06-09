// Custom JavaScript for Chronovyan documentation

document.addEventListener('DOMContentLoaded', function() {
    // Add copy buttons to code blocks
    document.querySelectorAll('pre > code').forEach(function(codeBlock) {
        // Create copy button
        const button = document.createElement('button');
        button.className = 'md-clipboard';
        button.title = 'Copy to clipboard';
        button.type = 'button';
        
        // Add icon
        const icon = document.createElement('span');
        icon.className = 'md-icon';
        icon.textContent = 'content_copy';
        button.appendChild(icon);
        
        // Add button to DOM
        const wrapper = document.createElement('div');
        wrapper.className = 'code-actions';
        wrapper.appendChild(button);
        codeBlock.parentNode.insertBefore(wrapper, codeBlock);
        
        // Add click handler
        button.addEventListener('click', function() {
            navigator.clipboard.writeText(codeBlock.textContent).then(function() {
                // Change icon to checkmark on success
                icon.textContent = 'check';
                setTimeout(function() {
                    icon.textContent = 'content_copy';
                }, 2000);
            });
        });
    });

    // Add smooth scrolling for anchor links
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

    // Add keyboard navigation for tabs
    document.querySelectorAll('.tabbed-set > input').forEach((input, i, inputs) => {
        input.addEventListener('keydown', e => {
            if (e.key === 'ArrowRight') {
                const next = inputs[i + 1] || inputs[0];
                next.checked = true;
                next.focus();
                e.preventDefault();
            } else if (e.key === 'ArrowLeft') {
                const prev = inputs[i - 1] || inputs[inputs.length - 1];
                prev.checked = true;
                prev.focus();
                e.preventDefault();
            } else if (e.key === 'Home') {
                inputs[0].checked = true;
                inputs[0].focus();
                e.preventDefault();
            } else if (e.key === 'End') {
                inputs[inputs.length - 1].checked = true;
                inputs[inputs.length - 1].focus();
                e.preventDefault();
            }
        });
    });

    // Add external link indicators
    document.querySelectorAll('a').forEach(link => {
        if (link.hostname && link.hostname !== window.location.hostname) {
            link.setAttribute('target', '_blank');
            link.setAttribute('rel', 'noopener noreferrer');
            
            // Add external link icon
            const icon = document.createElement('span');
            icon.className = 'external-link-icon';
            icon.innerHTML = '&nearr;';
            icon.setAttribute('aria-hidden', 'true');
            link.appendChild(icon);
            
            // Add screen reader text
            const srText = document.createElement('span');
            srText.className = 'sr-only';
            srText.textContent = ' (opens in new tab)';
            link.appendChild(srText);
        }
    });
});

// Add syntax highlighting for Chronovyan code blocks
window.MathJax = {
    tex: {
        inlineMath: [['$', '$'], ['\\(', '\\)']],
        displayMath: [['$$', '$$'], ['\\[', '\\]']],
        processEscapes: true,
        processEnvironments: true,
    },
    options: {
        skipHtmlTags: ['script', 'noscript', 'style', 'textarea', 'pre', 'code'],
        ignoreHtmlClass: 'tex2jax_ignore',
        processHtmlClass: 'tex2jax_process'
    },
    loader: {
        load: ['[tex]/mhchem', '[tex]/physics']
    },
    startup: {
        ready: function() {
            // Add custom macros here
            MathJax.startup.defaultReady();
        }
    }
};

// Initialize tooltips
function initTooltips() {
    const tooltipTriggerList = [].slice.call(document.querySelectorAll('[data-bs-toggle="tooltip"]'));
    tooltipTriggerList.map(function(tooltipTriggerEl) {
        return new bootstrap.Tooltip(tooltipTriggerEl);
    });
}

// Call init functions when the page loads
window.addEventListener('load', function() {
    initTooltips();
});
