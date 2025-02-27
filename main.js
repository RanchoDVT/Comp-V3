document.addEventListener("DOMContentLoaded", async () => {
    document.getElementById("year").textContent = new Date().getFullYear();
    const cache = new Map();

    // New helper function to handle caching and fetch processing.
    async function cachedFetch(url, processFn) {
        if (cache.has(url)) return cache.get(url);
        try {
            const response = await fetch(url, { cache: "force-cache" });
            if (!response.ok) throw new Error(`Failed to fetch ${url}`);
            const data = await processFn(response);
            cache.set(url, data);
            return data;
        } catch (err) {
            console.error(`Error fetching ${url}:`, err);
            return null;
        }
    }

    // Modified fetchFile using cachedFetch.
    async function fetchFile(url, targetElement) {
        const text = await cachedFetch(url, r => r.text());
        if (text !== null) targetElement.innerHTML = text;
    }

    // Modified fetchRepositories using cachedFetch.
    async function fetchRepositories(user) {
        const url = `https://api.github.com/users/${user}/repos`;
        const repos = await cachedFetch(url, r => r.json());
        return repos || [];
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

        if (selected === "comp-v3") {
            // Show the Comp V3 banner & config form
            document.getElementById("comp-v3-banner").style.display = "block";
            document.querySelector(".content").style.display = "block";
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

    // Helper: Show temporary popup message (e.g., at the center of viewport)
    function showTemporaryPopup(message) {
        const popup = document.createElement("div");
        popup.classList.add("temp-popup");
        popup.textContent = message;
        document.body.appendChild(popup);
        setTimeout(() => {
            popup.remove();
        }, 2000);
    }

    function enableMobileDropdowns() {
        // Only on small screens.
        if (/Mobi|Android/i.test(navigator.userAgent)) {
            const dropdownLinks = document.querySelectorAll("nav li.dropdown > a.nav-link");
            dropdownLinks.forEach(link => {
                link.addEventListener("click", function (e) {
                    const dropdown = this.nextElementSibling;
                    if (dropdown && dropdown.classList.contains("dropdown-content")) {
                        // If dropdown is not open, treat this as the first click.
                        if (dropdown.style.display !== "block") {
                            e.preventDefault();  // Prevent navigation on first click.
                            dropdown.style.display = "block";
                            showTemporaryPopup("Click again to open page...");
                            // Auto-close after 3 seconds if no second click.
                            setTimeout(() => {
                                dropdown.style.display = "none";
                            }, 3000);
                        } else {
                            // Second click: Allow navigation.
                            dropdown.style.display = "none";
                        }
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
});