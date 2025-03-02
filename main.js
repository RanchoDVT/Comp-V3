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

        loadMarked(async () => {
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
                        ${release.body ? window.marked.parse(release.body) : "<p>No release notes.</p>"}
                    </div>`;
                });
                outputHTML += `</section>`;
            } catch (err) {
                console.error(err);
                outputHTML += `<section><h2>Releases</h2><p>Unable to load releases.</p></section>`;
            }

            changelogElem.innerHTML = outputHTML;
        });
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

        // Update active state on banners
        const banners = document.querySelectorAll('.program-banner');
        banners.forEach(banner => {
            if (banner.dataset.project === selected) {
                banner.classList.add('active');
                banner.style.display = "block";
            } else {
                banner.classList.remove('active');
                // Hide other banners when one is selected
                banner.style.display = "none";
            }
        });

        // Hide roadmap by default
        const roadmapSection = document.getElementById("comp-v3-roadmap");
        if (roadmapSection) roadmapSection.style.display = "none";

        if (selected === "comp-v3") {
            // Show the config form
            document.querySelector(".content").style.display = "block";
            // Show roadmap for Comp V3
            if (roadmapSection) roadmapSection.style.display = "block";
            // Clear any project info
            document.getElementById("project-info").innerHTML = "";
        } else {
            // Hide Comp V3 config form
            document.querySelector(".content").style.display = "none";
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

    // Add interactive banner functionality
    function setupBannerSelectors() {
        const banners = document.querySelectorAll('.program-banner');

        banners.forEach(banner => {
            // Add text content if missing
            if (!banner.textContent.trim()) {
                const projectName = banner.id.replace('-banner', '').replace(/-/g, ' ');
                banner.textContent = projectName.charAt(0).toUpperCase() + projectName.slice(1);
            }

            banner.addEventListener('click', function (e) {
                // Ignore clicks on the status button (those are handled separately)
                if (e.target.classList.contains('status-btn')) {
                    return;
                }

                const projectValue = this.dataset.project;

                if (this.classList.contains('active') && !showingAllBanners) {
                    // If clicking on the active banner and not showing all banners,
                    // show all banners
                    showingAllBanners = true;
                    showAllBanners();
                } else {
                    // Otherwise select this banner and hide others
                    showingAllBanners = false;
                    selectBanner(projectValue);
                }
            });
        });
    }

    // Add these new functions
    function showAllBanners() {
        const banners = document.querySelectorAll('.program-banner');
        banners.forEach(banner => {
            banner.style.display = "block";
        });
    }

    function selectBanner(projectValue) {
        const banners = document.querySelectorAll('.program-banner');

        banners.forEach(banner => {
            if (banner.dataset.project === projectValue) {
                banner.classList.add('active');
                banner.style.display = "block";

                // Load project info
                loadProjectContent(projectValue);
            } else {
                banner.classList.remove('active');
                banner.style.display = "none";
            }
        });

        // Remove any back buttons if present
        const backButtons = document.querySelectorAll('#banner-container .program-banner:not([data-project])');
        backButtons.forEach(btn => btn.remove());
    }

    // New function to load appropriate content based on selected project
    function loadProjectContent(project) {
        // Hide/show the appropriate content sections
        const contentDiv = document.querySelector(".content");
        const projectInfoDiv = document.getElementById("project-info");

        if (project === "comp-v3") {
            contentDiv.style.display = "block";
            projectInfoDiv.innerHTML = "";
        } else {
            contentDiv.style.display = "none";
            window.loadProjectInfo(project, "project-info");
        }
    }

    // Replace the updateProjectView function with this simpler version
    window.updateProjectView = function () {
        // This function is now just a compatibility wrapper
        // for the banner-based navigation
        const activeBanner = document.querySelector('.program-banner.active');
        if (activeBanner) {
            selectBanner(activeBanner.dataset.project);
        } else {
            // Default to comp-v3 if no active banner
            selectBanner("comp-v3");
        }
    };

    // Call the setup function
    setupBannerSelectors();
});

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

// Add this function to show project status

window.showProjectStatus = function (project, event) {
    // Stop event propagation to prevent banner click
    if (event) {
        event.stopPropagation();
    }

    // Create popup content based on project
    let statusTitle = "Project Status";
    let statusContent = "";

    switch (project) {
        case 'comp-v3':
            statusTitle = "Comp V3 Status";
            statusContent = `
                <h3>Comp V3 Development Status</h3>
                <div class="status-info">
                    <p><strong>Status:</strong> Active Development</p>
                    <p><strong>Version:</strong> ${getLatestVersion("comp-v3") || "Loading..."}</p>
                    <p><a href="https://github.com/users/Voidless7125/projects/2" target="_blank">View Full Project Board</a></p>
                </div>
            `;
            break;

        case 'lockdown-browser':
            statusTitle = "Lockdown Browser Status";
            statusContent = `
                <h3>Lockdown Browser Status</h3>
                <div class="status-info">
                    <p><strong>Status:</strong> Stable Release</p>
                    <p><strong>Version:</strong> ${getLatestVersion("lockdown-browser") || "Loading..."}</p>
                    <p><a href="https://github.com/gucci-on-fleek/lockdown-browser/" target="_blank">View on GitHub</a></p>
                </div>
            `;
            break;

        case 'ois-rewrite':
            statusTitle = "OIS-Rewrite Status";
            statusContent = `
                <h3>OIS-Rewrite Status</h3>
                <div class="status-info">
                    <p><strong>Status:</strong> In Development</p>
                    <p><strong>Version:</strong> ${getLatestVersion("ois-rewrite") || "Loading..."}</p>
                    <p><a href="https://github.com/Voidless7125/OIS-Rewrite" target="_blank">View on GitHub</a></p>
                </div>
            `;
            break;

        case 'ois-decompile':
            statusTitle = "OIS-Decompile Status";
            statusContent = `
                <h3>OIS-Decompile Status</h3>
                <div class="status-info">
                    <p><strong>Status:</strong> Archive</p>
                    <p><strong>Version:</strong> ${getLatestVersion("ois-decompile") || "Loading..."}</p>
                    <p><a href="https://github.com/Voidless7125/OIS-Decompile" target="_blank">View on GitHub</a></p>
                </div>
            `;
            break;
    }

    // Set popup content
    document.getElementById("popup-title").textContent = statusTitle;
    document.getElementById("popup-text").innerHTML = statusContent;

    // Show popup
    document.getElementById("popup").classList.add("active");
    document.getElementById("overlay").classList.add("active");

    // Use appropriate function for the download button or hide it
    const downloadBtn = document.getElementById("download-link");
    downloadBtn.style.display = "none"; // Hide download button for status popup
};

// Helper function to get latest version
function getLatestVersion(project) {
    // Return from cache if available
    if (window.projectVersions && window.projectVersions[project]) {
        return window.projectVersions[project];
    }

    // Initialize cache if not exists
    if (!window.projectVersions) {
        window.projectVersions = {};
    }

    // Get repo path based on project
    let repoPath = "";
    switch (project) {
        case 'comp-v3':
            repoPath = "Voidless7125/Comp-V3";
            break;
        case 'lockdown-browser':
            repoPath = "gucci-on-fleek/lockdown-browser";
            break;
        case 'ois-rewrite':
            repoPath = "Voidless7125/OIS-Rewrite";
            break;
        case 'ois-decompile':
            repoPath = "Voidless7125/OIS-Decompile";
            break;
    }

    // Fetch latest version
    if (repoPath) {
        fetch(`https://api.github.com/repos/${repoPath}/releases/latest`)
            .then(response => response.json())
            .then(data => {
                if (data && data.tag_name) {
                    window.projectVersions[project] = data.tag_name;

                    // Update version in popup if currently open
                    const popup = document.getElementById("popup");
                    if (popup.classList.contains("active")) {
                        const versionElement = popup.querySelector(".status-info p:nth-child(2)");
                        if (versionElement) {
                            versionElement.innerHTML = `<strong>Version:</strong> ${data.tag_name}`;
                        }
                    }
                }
            })
            .catch(err => console.error("Error fetching version:", err));
    }

    return "Checking...";
}

// Add this to the end of your file, outside any function
// Better service worker registration with path adjustment based on environment
if ('serviceWorker' in navigator) {
    window.addEventListener('load', () => {
        // Check if running on localhost
        const isLocalhost = window.location.hostname === 'localhost' ||
            window.location.hostname === '127.0.0.1';

        // Set the appropriate service worker path
        const swPath = isLocalhost ? '/service-worker.js' : '/Comp-V3/service-worker.js';

        navigator.serviceWorker.register(swPath)
            .then(registration => {
                console.log('ServiceWorker registered with scope:', registration.scope);
            })
            .catch(error => {
                console.error('ServiceWorker registration failed:', error);
            });
    });
}