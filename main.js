let siteCache;

document.addEventListener("DOMContentLoaded", async () => {

    document.getElementById("year").textContent = new Date().getFullYear();
    // Change this line to assign to the global variable
    siteCache = new Map();

    // New helper function to handle caching and fetch processing.
    async function cachedFetch(url, processFn) {
        if (siteCache.has(url)) return siteCache.get(url);
        try {
            const response = await fetch(url, { cache: "force-cache" });
            if (!response.ok) throw new Error(`Failed to fetch ${url}`);
            const data = await processFn(response);
            siteCache.set(url, data);
            return data;
        } catch (err) {
            console.error(`Error fetching ${url}:`, err);
            return null;
        }
    }

    // Add this function to append version query parameters to assets
    function getVersionedAssetUrl(assetPath) {
        const commitHash = document.cookie.match(/compV3CommitHash=([^;]+)/)?.[1] || Date.now();
        return `${assetPath}?v=${commitHash}`;
    }

    // Modified fetchFile using cachedFetch.
    async function fetchFile(url, targetElement) {
        const versionedUrl = getVersionedAssetUrl(url);
        const text = await cachedFetch(versionedUrl, r => r.text());
        if (text !== null) targetElement.innerHTML = text;
    }

    // Modified fetchRepositories using cachedFetch.
    async function fetchRepositories(user) {
        try {
            const url = `https://api.github.com/users/${user}/repos`;
            const response = await fetch(url, { cache: "force-cache" });

            if (response.status === 403) {
                // GitHub API rate limit exceeded
                const projectsDropdown = document.getElementById("projects-dropdown");
                if (projectsDropdown) {
                    const warningEl = document.createElement("div");
                    warningEl.className = "rate-limit-warning";
                    warningEl.textContent = "⚠️ GitHub API rate limit exceeded. Some repositories may not be shown.";
                    warningEl.style.padding = "10px";
                    warningEl.style.color = "#ffcc00";
                    warningEl.style.backgroundColor = "rgba(50,50,50,0.9)";
                    projectsDropdown.prepend(warningEl);
                }
                return [];
            }

            if (!response.ok) throw new Error(`Failed to fetch ${url}`);
            return await response.json();
        } catch (err) {
            console.error(`Error fetching repositories for ${user}:`, err);
            return [];
        }
    }

    // Modified getLatestRelease using cachedFetch.
    async function getLatestRelease(repo) {
        const url = `https://api.github.com/repos/${repo}/releases/latest`;
        const releaseData = await cachedFetch(url, r => r.json());
        return releaseData ? releaseData.tag_name : "Unknown";
    }

    async function populateProjectsDropdown() {
        const users = ["Voidless7125"];
        const projectsDropdown = document.getElementById("projects-dropdown");
        const fragment = document.createDocumentFragment();

        const repoRequests = users.map(fetchRepositories);
        const userRepos = (await Promise.all(repoRequests)).flat();

        userRepos.forEach(repo => {
            if (![".github", "Voidless7125", "shh"].includes(repo.name)) {
                const repoLink = document.createElement("a");
                repoLink.href = repo.html_url;
                repoLink.target = "_blank";
                repoLink.textContent = repo.name;
                fragment.appendChild(repoLink);
            }
        });

        const sponsoredLink = document.createElement("a");
        sponsoredLink.href = "https://github.com/gucci-on-fleek/lockdown-browser/";
        sponsoredLink.target = "_blank";
        sponsoredLink.textContent = "✨ Lockdown Browser";
        fragment.insertBefore(sponsoredLink, fragment.firstChild);

        projectsDropdown.appendChild(fragment);
    }

    // New changelog loader: releases plus legacy changelog
    async function loadChangelog() {
        const changelogElem = document.getElementById("changelog-content");
        if (!changelogElem) return;
        let outputHTML = "";

        // Fetch releases from GitHub API
        try {
            const releasesResponse = await fetch("https://api.github.com/repos/Voidless7125/Comp-V3/releases", { cache: "force-cache" });
            const releasesData = await releasesResponse.json();
            if (!releasesResponse.ok) {
                throw new Error("Failed to load releases");
            }
            outputHTML += `<section><h2>Releases</h2>`;
            releasesData.forEach(release => {
                outputHTML += `<div class="release">
                    <h3>${release.name || release.tag_name}</h3>
                    ${release.body ? marked.parse(release.body) : "<p>No release notes.</p>"}
                </div>`;
            });
            outputHTML += `</section>`;
        } catch (err) {
            console.error(err);
            outputHTML += `<section><h2>Releases</h2><p>Unable to load releases.</p></section>`;
        }

        // Fetch legacy changelog content
        try {
            const legacyResponse = await fetch("https://raw.githubusercontent.com/Voidless7125/Comp-V3/dev/legacy_changelog.md");
            if (!legacyResponse.ok) {
                throw new Error("Failed to load legacy changelog");
            }
            const legacyText = await legacyResponse.text();
            outputHTML += `<section>
      <h2>Legacy Changelog</h2>
      ${marked.parse(legacyText)}
    </section>`;
        } catch (err) {
            console.error(err);
            outputHTML += `<section>
      <h2>Legacy Changelog</h2>
      <p>Unable to load legacy changelog.</p>
    </section>`;
        }

        changelogElem.innerHTML = outputHTML;
    }

    if (document.getElementById("readme-content")) {
        await fetchFile("https://raw.githubusercontent.com/Voidless7125/Comp-V3/dev/README.md", document.getElementById("readme-content"));
    }
    if (document.getElementById("changelog-content")) {
        await loadChangelog();
    }

    async function loadContent() {
        const navbar = document.querySelector("#navbar");

        if (navbar) {
            await fetchFile("navbar.html", navbar);
            const currentPage = window.location.pathname.split("/").pop() || "index.html";
            navbar.querySelectorAll("nav a.nav-link").forEach((link) => {
                if (link.dataset.page === currentPage) link.classList.add("active");
            });
            populateProjectsDropdown();
        }
    }
    const form = document.getElementById("config-form");
    if (form) {
        form.addEventListener("submit", async (event) => {
            event.preventDefault();
            const formData = new FormData(form);
            document.getElementById("config-output").textContent = `
MOTOR_CONFIG {
    FRONT_LEFT_MOTOR { 
        PORT=${formData.get("front_left_port")} 
        GEAR_RATIO=${formData.get("front_left_gear_ratio")} 
        REVERSED=${formData.has("front_left_reversed")} 
    }

    FRONT_RIGHT_MOTOR { 
        PORT=${formData.get("front_right_port")} 
        GEAR_RATIO=${formData.get("front_right_gear_ratio")} 
        REVERSED=${formData.has("front_right_reversed")} 
    }

    REAR_LEFT_MOTOR { 
        PORT=${formData.get("rear_left_port")} 
        GEAR_RATIO=${formData.get("rear_left_gear_ratio")} 
        REVERSED=${formData.has("rear_left_reversed")} 
    }

    REAR_RIGHT_MOTOR { 
        PORT=${formData.get("rear_right_port")} 
        GEAR_RATIO=${formData.get("rear_right_gear_ratio")} 
        REVERSED=${formData.has("rear_right_reversed")} 
    }

    INERTIAL { 
        PORT=${formData.get("inertial_port")} 
    }
    
    Rear_Bumper { 
        PORT=${formData.get("rear_bumper_port")} 
    } 
}

PRINTLOGO=${formData.has("print_logo")}
LOGTOFILE=${formData.has("log_to_file")}
MAXOPTIONSSIZE=${formData.get("max_options_size")}
POLLINGRATE=${formData.get("polling_rate")}
CTRLR1POLLINGRATE=${formData.get("ctrlr1_polling_rate")}
CONFIGTYPE=${formData.get("config_type")}
TEAMNUMBER=${formData.get("team_number")}
LOADINGGIFPATH=${formData.get("loading_gif_path")}
AUTOGIFPATH=${formData.get("auto_gif_path")}
DRIVEGIFPATH=${formData.get("drive_gif_path")}
CUSTOMMESSAGE=${formData.get("custom_message")}
DRIVEMODE=${formData.get("drive_mode")}
VERSION=${await getLatestRelease("Voidless7125/Comp-V3")}`;
            document.getElementById("copy-button").style.display = "inline-block";

            // Change the submit button text to "Regenerate Config"
            const submitButton = form.querySelector('button[type="submit"]');
            if (submitButton) {
                submitButton.textContent = "Regenerate Config";
            }
        });
    }

    if (document.getElementById("copy-button")) {
        document
            .getElementById("copy-button")
            .addEventListener("click", function () {
                const configOutput = document.getElementById("config-output");
                if (configOutput.textContent) {
                    navigator.clipboard
                        .writeText(configOutput.textContent)
                        .then(() => {
                            const copyButton = document.getElementById("copy-button");
                            copyButton.textContent = "Copied! ✅";
                            setTimeout(() => {
                                copyButton.textContent = "Recopy";
                            }, 2000);
                        })
                        .catch((err) => console.error("Error copying text:", err));
                }
            });
    }

    async function getDownloadLinkFor(type) {
        let downloadLink = "#";
        let popupTitleText = "";
        let popupBodyHTML = "";

        if (type === "Dev") {
            popupTitleText = "Download Comp-V3 Dev";
            popupBodyHTML =
                "Thank you for downloading Comp-V3 dev. This download is the source code. You will need my Custom SDK to use it.";
            downloadLink =
                "https://github.com/Voidless7125/Comp-V3/archive/refs/heads/dev.zip";
        } else if (type === "Stable") {
            const latestTag = await getLatestRelease("Voidless7125/Comp-V3");
            popupTitleText = `Download Comp-V3 Stable | ${latestTag}`;
            popupBodyHTML =
                "Thank you for downloading Comp-V3 stable. This download is the source code. You will need my Custom SDK to use it.";
            downloadLink = `https://github.com/Comp-V3/archive/refs/tags/${latestTag}.zip`;
            if (!latestTag || latestTag === "Unknown") {
                downloadLink = "#";
            }
        } else if (type === "SDK") {
            const latestTag = await getLatestRelease("Voidless7125/Vex-SDK");
            popupTitleText = `Download Custom SDK | ${latestTag}`;
            popupBodyHTML =
                'Thank you for downloading my custom SDK. <br><strong>This is unofficial and in no way affiliated with VEX Robotics. Please contact me for more info.</strong><br><a target="_blank" href="https://github.com/Voidless7125/Vex-SDK"><br>Source code</a><br><br><strong>The source code might not download correctly – you may have to use git clone.</strong><br>The download button is the simple powershell script.';
            downloadLink = `https://github.com/Voidless7125/Vex-SDK/blob/dev/Vex-SDK.updater.ps1`;
            if (!latestTag || latestTag === "Unknown") {
                downloadLink = "#";
            }
        }
        return { downloadLink, popupTitleText, popupBodyHTML };
    }

    window.showPopup = async (type) => {
        const { downloadLink, popupTitleText, popupBodyHTML } =
            await getDownloadLinkFor(type);
        const popupTitle = document.getElementById("popup-title");
        const popupText = document.getElementById("popup-text");
        const downloadLinkElem = document.getElementById("download-link");

        popupTitle.textContent = popupTitleText;
        popupText.innerHTML = (typeof DOMPurify !== 'undefined') ? DOMPurify.sanitize(popupBodyHTML) : popupBodyHTML;
        downloadLinkElem.href = downloadLink;

        if (downloadLink === "#") {
            disableDownloadButton(downloadLinkElem);
        } else {
            downloadLinkElem.classList.remove("disabled-download");
            downloadLinkElem.style.backgroundColor = "";
            downloadLinkElem.textContent = "Download";
        }

        document.getElementById("popup").classList.add("active");
        document.getElementById("overlay").classList.add("active");
    };

    function disableDownloadButton(button) {
        button.classList.add("disabled-download");
        button.removeAttribute("href");
        button.style.backgroundColor = "gray";
        button.textContent = "Download Unavailable";
    }

    window.hidePopup = () => {
        document.getElementById("popup").classList.remove("active");
        document.getElementById("overlay").classList.remove("active");
    };

    window.loadProjectInfo = async function (project, targetElementId) {
        const infoDiv = document.getElementById(targetElementId);
        if (!infoDiv) {
            console.error(`Element with id "${targetElementId}" not found.`);
            return;
        }

        // Decide which URLs to use based on the project
        let readmeUrl = "";
        let releasesUrl = "";
        if (project === "lockdown-browser") {
            readmeUrl = "https://raw.githubusercontent.com/gucci-on-fleek/lockdown-browser/master/Readme.md";
            releasesUrl = "https://api.github.com/repos/gucci-on-fleek/lockdown-browser/releases";
        } else if (project === "ois-rewrite") {
            readmeUrl = "https://raw.githubusercontent.com/Voidless7125/OIS-Rewrite/master/README.md";
            releasesUrl = "https://api.github.com/repos/Voidless7125/OIS-Rewrite/releases";
        } else if (project === "ois-decompile") {
            readmeUrl = "https://raw.githubusercontent.com/Voidless7125/OIS-Decompile/master/README.md";
            releasesUrl = "https://api.github.com/repos/Voidless7125/OIS-Decompile/releases";
        } else {
            console.error("Invalid project provided.");
            return;
        }

        let outputHTML = "";

        // Fetch and parse the README
        try {
            const readmeResponse = await fetch(readmeUrl);
            if (!readmeResponse.ok) {
                throw new Error("Failed to load README");
            }
            const readmeText = await readmeResponse.text();
            const readmeHTML = marked.parse(readmeText);
            outputHTML += `<section>
            <h2>README</h2>
            ${readmeHTML}
          </section>`;
        } catch (err) {
            outputHTML += `<section>
            <h2>README</h2>
            <p>Unable to load README for ${project}.</p>
          </section>`;
        }

        // Fetch and parse all releases
        try {
            const releasesResponse = await fetch(releasesUrl);
            if (!releasesResponse.ok) {
                throw new Error("Failed to load releases");
            }
            const releasesData = await releasesResponse.json();
            if (releasesData.length > 0) {
                outputHTML += `<section>
              <h2>Releases</h2>`;
                releasesData.forEach(release => {
                    // Parse markdown in the release body (if available)
                    const releaseBody = release.body ? marked.parse(release.body) : "<p>No release notes.</p>";
                    outputHTML += `<div class="release">
                <h3>${release.name || release.tag_name}</h3>
                ${releaseBody}
                <p><a href="${release.html_url}" target="_blank">View Release on GitHub</a></p>
              </div>`;
                });
                outputHTML += `</section>`;
            } else {
                outputHTML += `<section>
              <h2>Releases</h2>
              <p>No releases available.</p>
            </section>`;
            }
        } catch (err) {
            outputHTML += `<section>
            <h2>Releases</h2>
            <p>Unable to load releases for ${project}.</p>
          </section>`;
        }

        infoDiv.innerHTML = outputHTML;
    };

    window.updateProjectView = function () {
        const projectSelect = document.getElementById("project");
        const selected = projectSelect.value;

        // Hide all banners first
        const banners = document.querySelectorAll(".program-banner");
        banners.forEach(banner => banner.style.display = "none");

        // Hide roadmap by default
        const roadmapSection = document.getElementById("comp-v3-roadmap");
        if (roadmapSection) roadmapSection.style.display = "none";

        if (selected === "comp-v3") {
            // Show the Comp V3 banner & config form
            document.getElementById("comp-v3-banner").style.display = "block";
            document.querySelector(".content").style.display = "block";
            // Show roadmap for Comp V3
            if (roadmapSection) roadmapSection.style.display = "block";
            // Clear any project info
            document.getElementById("project-info").innerHTML = "";
        } else {
            // Hide Comp V3 config form
            document.querySelector(".content").style.display = "none";
            // Show the banner matching the selected program
            const bannerId = selected + "-banner";
            const bannerElem = document.getElementById(bannerId);
            if (bannerElem) {
                bannerElem.style.display = "block";
            }
            // Load the project info (GitHub releases + README)
            window.loadProjectInfo(selected, "project-info");
        }
    };

    function enableMobileDropdowns() {
        // Only on mobile devices
        if (/Mobi|Android/i.test(navigator.userAgent)) {
            const dropdownLinks = document.querySelectorAll("nav li.dropdown > a.nav-link");
            dropdownLinks.forEach(link => {
                link.addEventListener("click", function (e) {
                    const dropdown = this.nextElementSibling;
                    if (dropdown && dropdown.classList.contains("dropdown-content")) {
                        // Only prevent default if this is a dropdown toggle WITHOUT href
                        if (!this.getAttribute("href") || this.getAttribute("href") === "#") {
                            e.preventDefault();
                            dropdown.style.display = dropdown.style.display === "block" ? "none" : "block";
                        }
                        // If it has an href, let the browser handle navigation normally
                    }
                });
            });
        }
    }

    function debounce(func, wait) {
        let timeout;
        return function (...args) {
            clearTimeout(timeout);
            timeout = setTimeout(() => func.apply(this, args), wait);
        };
    }

    window.addEventListener("resize", debounce(enableMobileDropdowns, 200));
    await loadContent();
    enableMobileDropdowns();
    // Call updateProjectView to set the default view (comp-v3)
    if (document.getElementById("project")) {
        window.updateProjectView();
    }

    function initGlowCursor() {
        const blob = document.createElement('div');
        blob.id = 'cursor-blob';
        document.body.appendChild(blob);

        // Position the blob initially at center of screen
        const initialX = window.innerWidth / 2;
        const initialY = window.innerHeight / 2;
        blob.style.left = `${initialX}px`;
        blob.style.top = `${initialY}px`;
        blob.style.opacity = '1';
        blob.style.display = 'block';

        // Performance optimization: Store DOM elements and track animation state
        let isAnimating = false;
        let pendingCursorX = initialX;
        let pendingCursorY = initialY;

        // Cache DOM elements for better performance
        let navbar = null;
        let navbarItems = [];

        // Initial element caching
        function refreshElementCache() {
            navbar = document.querySelector('#navbar');
            if (navbar) {
                // Modified selector to better capture ALL dropdown items
                navbarItems = Array.from(document.querySelectorAll('nav a, nav .nav-link, .dropdown-content a'));
            }

            // Only track elements currently visible in viewport (big performance win)
            visibleTextElements = Array.from(document.querySelectorAll('p, h1, h2, h3, h4, h5, h6, span, label, li')).filter(el => {
                const rect = el.getBoundingClientRect();
                return !(rect.bottom < 0 || rect.top > window.innerHeight ||
                    rect.right < 0 || rect.left > window.innerWidth);
            });
        }

        // Initial cache population
        refreshElementCache();

        // Refresh element cache occasionally and on key events
        window.addEventListener('resize', debounce(refreshElementCache, 200));
        window.addEventListener('scroll', debounce(refreshElementCache, 200));
        setInterval(refreshElementCache, 5000); // Refresh every 5 seconds

        // Performance-optimized pointer move handler
        document.addEventListener('pointermove', event => {
            pendingCursorX = event.clientX;
            pendingCursorY = event.clientY;
            window.currentMouseX = pendingCursorX;
            window.currentMouseY = pendingCursorY;
            window.lastMouseMoveTime = Date.now();

            if (!isAnimating) {
                isAnimating = true;
                requestAnimationFrame(updateCursor);
            }
        });

        // Counter to reduce frequency of effect updates

        function updateCursor() {
            // Get the blob's dimensions
            const blobWidth = blob.offsetWidth;
            const blobHeight = blob.offsetHeight;

            // Animate the blob position, centering it on the cursor
            blob.animate({
                left: `${pendingCursorX - (blobWidth / 2)}px`,
                top: `${pendingCursorY - (blobHeight / 2)}px`
            }, {
                duration: 3000,
                fill: "forwards"
            });

            checkNavbarGlow(pendingCursorX, pendingCursorY);

            // Allow next animation frame
            isAnimating = false;
        }

        function checkNavbarGlow(cursorX, cursorY) {
            if (!navbar) return;

            // For background glow effect
            const navbarRect = navbar.getBoundingClientRect();
            const navbarCenterX = navbarRect.left + navbarRect.width / 2;
            const navbarCenterY = navbarRect.top + navbarRect.height / 2;

            const navDistance = Math.sqrt(
                Math.pow(cursorX - navbarCenterX, 2) +
                Math.pow(cursorY - navbarCenterY, 2)
            );

            const backgroundGlowThreshold = 300;

            if (navDistance < backgroundGlowThreshold) {
                const bgIntensity = 1 - (navDistance / backgroundGlowThreshold);
                const glowColor = `rgba(60, 100, 255, ${bgIntensity * 0.15})`;
                navbar.style.boxShadow = `0 0 ${Math.floor(40 * bgIntensity)}px ${glowColor}`;
                navbar.style.backgroundColor = `rgba(30, 40, 60, ${0.7 + (bgIntensity * 0.3)})`;
            } else {
                navbar.style.boxShadow = '';
                navbar.style.backgroundColor = '';
            }

            // Apply text glow effect to navbar items
            const textGlowThreshold = 200;

            for (let i = 0; i < navbarItems.length; i++) {
                const item = navbarItems[i];
                const rect = item.getBoundingClientRect();

                // Skip if not in viewport
                if (rect.bottom < 0 || rect.top > window.innerHeight ||
                    rect.right < 0 || rect.left > window.innerWidth) {
                    continue;
                }

                const itemCenterX = rect.left + rect.width / 2;
                const itemCenterY = rect.top + rect.height / 2;

                const distance = Math.sqrt(
                    Math.pow(cursorX - itemCenterX, 2) +
                    Math.pow(cursorY - itemCenterY, 2)
                );

                if (distance < textGlowThreshold) {
                    const intensity = 1 - (distance / textGlowThreshold);
                    const glowColor = `rgba(255, 255, 255, ${intensity * 0.8})`;
                    const glowSize = Math.max(5, Math.floor(20 * intensity));

                    item.style.textShadow = `0 0 ${glowSize}px ${glowColor}`;

                    const colorBoost = Math.floor(intensity * 50);
                    item.style.color = `rgb(${255 - colorBoost}, ${255 - colorBoost}, 255)`;
                } else {
                    // Only reset if previously set
                    if (item.style.textShadow || item.style.color) {
                        item.style.textShadow = '';
                        item.style.color = '';
                    }
                }
            }
        };
    }

    initGlowCursor();


    // Add this to your DOMContentLoaded event handler
    if (document.getElementById("wiki-features")) {
        loadWikiFeatures();
    }

    // Add this to your DOMContentLoaded event handler
    if (document.getElementById("comp-v3-roadmap")) {
        loadCompV3Roadmap();
    }

    // Add this inside the DOMContentLoaded event handler
    window.clearAllSiteData = function () {
        const message = clearSiteData();
        if (confirm(message)) {
            window.location.reload();
        }
    };
});

// Add this function to fetch and display wiki content
async function loadWikiFeatures() {
    const wikiContainer = document.getElementById("wiki-features");
    if (!wikiContainer) return;

    try {
        // Fetch wiki content from GitHub API
        const response = await fetch("https://api.github.com/repos/Voidless7125/Comp-V3/wiki");
        if (!response.ok) throw new Error("Failed to load wiki data");

        const wikiPages = await response.json();
        const featuresPage = wikiPages.find(page => page.title.toLowerCase().includes("features"));

        if (featuresPage) {
            // Fetch the content of the features wiki page
            const contentResponse = await fetch(featuresPage.html_url + ".md");
            if (!contentResponse.ok) throw new Error("Failed to load wiki content");

            const content = await contentResponse.text();
            wikiContainer.innerHTML = `
                <h2>Additional Features from Wiki</h2>
                <div class="wiki-content">
                    ${marked.parse(content)}
                </div>
            `;
        } else {
            wikiContainer.innerHTML = `<p>No features wiki page found. <a href="https://github.com/Voidless7125/Comp-V3/wiki" target="_blank">View all wiki pages</a>.</p>`;
        }
    } catch (error) {
        console.error("Error loading wiki features:", error);
        wikiContainer.innerHTML = `
            <p>Could not load features from wiki. <a href="https://github.com/Voidless7125/Comp-V3/wiki" target="_blank">View wiki directly</a>.</p>
        `;
    }
}

// Add this function to fetch and display roadmap
async function loadCompV3Roadmap() {
    const roadmapContainer = document.getElementById("comp-v3-roadmap");
    if (!roadmapContainer) return;

    try {
        // Since the GitHub Projects API requires authentication,
        // we'll provide a link instead of trying to embed it
        roadmapContainer.innerHTML = `
            <h3>Comp V3 Development Roadmap</h3>
            <p>View the complete development roadmap and progress on GitHub:</p>
            <a href="https://github.com/users/Voidless7125/projects/2" 
               target="_blank" 
               class="roadmap-link">
                Open Comp V3 Roadmap on GitHub
            </a>
        `;
    } catch (error) {
        console.error("Error with roadmap:", error);
    }
}

// Add this function to your existing code
function clearSiteData() {
    // Clear the in-memory cache Map
    if (siteCache) {
        siteCache.clear();
        console.log("In-memory cache cleared");
    } else {
        console.warn("Cache not available");
    }

    // Clear all cookies for this domain
    const cookies = document.cookie.split(";");
    for (let i = 0; i < cookies.length; i++) {
        const cookie = cookies[i];
        const eqPos = cookie.indexOf("=");
        const name = eqPos > -1 ? cookie.substr(0, eqPos).trim() : cookie.trim();
        document.cookie = name + "=;expires=Thu, 01 Jan 1970 00:00:00 GMT;path=/";
    }
    console.log("Cookies cleared");

    // Clear localStorage and sessionStorage
    if (window.localStorage) {
        localStorage.clear();
        console.log("Local storage cleared");
    }

    if (window.sessionStorage) {
        sessionStorage.clear();
        console.log("Session storage cleared");
    }

    // Force reload the page to apply changes
    localStorage.setItem('dataCleared', 'true');
    window.location.reload(true); // Force reload from server, not cache
    return "Site data cleared. Reloading page...";
}