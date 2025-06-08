// Theme JavaScript for Forever Code Documentation

document.addEventListener('DOMContentLoaded', function() {
  // Initialize WebFont Loader
  WebFont.load({
    google: {
      families: [
        'Inter:300,400,500,600,700,800',
        'Montserrat:700,800',
        'Fira Code:400,700&display=swap'
      ]
    },
    active: function() {
      // Fonts have been loaded
      document.documentElement.classList.add('fonts-loaded');
    }
  });

  // Add smooth scrolling for anchor links
  document.querySelectorAll('a[href^="#"]').forEach(anchor => {
    anchor.addEventListener('click', function(e) {
      e.preventDefault();
      
      const targetId = this.getAttribute('href');
      if (targetId === '#') return;
      
      const targetElement = document.querySelector(targetId);
      if (targetElement) {
        targetElement.scrollIntoView({
          behavior: 'smooth',
          block: 'start'
        });
        
        // Update URL without jumping
        if (history.pushState) {
          history.pushState(null, null, targetId);
        } else {
          location.hash = targetId;
        }
      }
    });
  });

  // Add copy button to code blocks
  document.querySelectorAll('.highlight').forEach(highlight => {
    // Create button
    const button = document.createElement('button');
    button.className = 'md-clipboard md-icon';
    button.type = 'button';
    button.setAttribute('aria-label', 'Copy code');
    
    // Add icon
    const icon = document.createElement('span');
    icon.className = 'md-icon';
    icon.textContent = 'content_copy';
    button.appendChild(icon);
    
    // Add button to code block
    const wrapper = document.createElement('div');
    wrapper.className = 'code-actions';
    wrapper.appendChild(button);
    
    highlight.parentNode.insertBefore(wrapper, highlight);
    
    // Add click event
    button.addEventListener('click', function() {
      const code = highlight.querySelector('code');
      if (code) {
        navigator.clipboard.writeText(code.textContent).then(() => {
          // Change icon to checkmark on success
          icon.textContent = 'check';
          setTimeout(() => {
            icon.textContent = 'content_copy';
          }, 2000);
        }).catch(err => {
          console.error('Failed to copy text: ', err);
        });
      }
    });
  });

  // Add active class to current section in navigation
  const currentPath = window.location.pathname;
  document.querySelectorAll('.md-nav__link').forEach(link => {
    if (link.getAttribute('href') === currentPath) {
      link.classList.add('md-nav__link--active');
      
      // Expand parent items
      let parent = link.parentElement;
      while (parent && !parent.classList.contains('md-nav__list')) {
        if (parent.classList.contains('md-nav__item--nested')) {
          const toggle = parent.querySelector('.md-nav__toggle');
          if (toggle) toggle.checked = true;
        }
        parent = parent.parentElement;
      }
    }
  });
});

// Add class to body when page is scrolled
window.addEventListener('scroll', function() {
  if (window.scrollY > 10) {
    document.body.classList.add('scrolled');
  } else {
    document.body.classList.remove('scrolled');
  }
});

// Initialize service worker for offline support
if ('serviceWorker' in navigator) {
  window.addEventListener('load', () => {
    navigator.serviceWorker.register('/sw.js').then(registration => {
      console.log('ServiceWorker registration successful');
    }).catch(err => {
      console.log('ServiceWorker registration failed: ', err);
    });
  });
}
