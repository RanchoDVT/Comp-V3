document.addEventListener('DOMContentLoaded', () => {
    document.getElementById("year").innerHTML = new Date().getFullYear();

    async function fetchFile(url, targetElement) {
        try {
            const response = await fetch(url);
            if (!response.ok) throw new Error(`${url} not found`);
            const text = await response.text();
            targetElement.innerHTML = text;
        } catch (error) {
            console.error(`Error fetching ${url}:`, error);
        }
    }

    async function fetchRepositories(user) {
        try {
            const response = await fetch(`https://api.github.com/users/${user}/repos`);
            if (!response.ok) throw new Error(`Error fetching repos for ${user}: ${response.status}`);
            const repos = await response.json();
            return repos;
        } catch (error) {
            console.error(`Error fetching repositories for ${user}:`, error);
            return [];
        }
    }

    async function populateProjectsDropdown() {
        const users = ['Voidless7125'];
        const projectsDropdown = document.getElementById('projects-dropdown');

        for (const user of users) {
            const repos = await fetchRepositories(user);
            repos.forEach(repo => {
                if (repo.name !== '.github' && repo.name !== 'Voidless7125' && repo.name !== 'shh') { // Exclude .github and Voidless7125 repositories
                    const repoLink = document.createElement('a');
                    repoLink.href = repo.html_url;
                    repoLink.target = '_blank';
                    repoLink.textContent = repo.name;
                    projectsDropdown.appendChild(repoLink);
                }
            });
        }
        const sponsoredIcon = document.createElement('span');
        sponsoredIcon.textContent = '✨ Lockdown Browser'; // Sparkle emoji instead of an image
        const sponsoredLink = document.createElement('a');
        sponsoredLink.href = 'https://github.com/gucci-on-fleek/lockdown-browser/'; // Replace with sponsor's URL
        sponsoredLink.target = '_blank';
        sponsoredLink.appendChild(sponsoredIcon);
        projectsDropdown.insertBefore(sponsoredLink, projectsDropdown.firstChild);
    }

    fetch('navbar.html')
        .then(function (res) {
            return res.text();
        })
        .then(function (text) {
            (document.querySelector("div#navbar")).innerHTML = text;
            const currentPage = window.location.pathname.split("/").pop() || "index.html";
            (document.querySelector("div#navbar")).querySelectorAll("nav a.nav-link").forEach(link => {
                if (link.dataset.page === currentPage) {
                    link.classList.add("active");
                }
            });
            populateProjectsDropdown(); // Populate the projects dropdown after loading the navbar
        });

    if (document.getElementById('readme-content')) fetchFile('https://raw.githubusercontent.com/RanchoDVT/Comp-V3/dev/README.md', document.getElementById('readme-content'));
    if (document.getElementById('changelog-content')) fetchFile('https://raw.githubusercontent.com/RanchoDVT/Comp-V3/dev/changelog.md', document.getElementById('changelog-content'));

    async function getLatestRelease(repo) {
        try {
            const response = await fetch(`https://api.github.com/repos/${repo}/releases/latest`);
            if (!response.ok) throw new Error(`Error fetching release info: ${response.status}`);
            const data = await response.json();
            return data.tag_name;
        } catch (error) {
            console.error('Error fetching latest release:', error);
            return 'Unknown';
        }
    }

    if (document.getElementById('config-form')) {
        (document.getElementById('config-form')).addEventListener('submit', async function (event) {
            event.preventDefault();
            document.getElementById('submit').innerHTML = 'Regenerate Config';
            const formData = new FormData((document.getElementById('config-form')));

            (document.getElementById('config-output')).textContent = `
MOTOR_CONFIG {
    FRONT_LEFT_MOTOR { 
    PORT=${formData.get('front_left_port')} 
    GEAR_RATIO=${formData.get('front_left_gear_ratio')} 
    REVERSED=${formData.has('front_left_reversed')} 
    }

    FRONT_RIGHT_MOTOR { 
    PORT=${formData.get('front_right_port')} 
    GEAR_RATIO=${formData.get('front_right_gear_ratio')} 
    REVERSED=${formData.has('front_right_reversed')} 
    }

    REAR_LEFT_MOTOR { 
    PORT=${formData.get('rear_left_port')} 
    GEAR_RATIO=${formData.get('rear_left_gear_ratio')} 
    REVERSED=${formData.has('rear_left_reversed')} 
    }

    REAR_RIGHT_MOTOR { 
    PORT=${formData.get('rear_right_port')} 
    GEAR_RATIO=${formData.get('rear_right_gear_ratio')} 
    REVERSED=${formData.has('rear_right_reversed')} 
    }

    INERTIAL { 
    PORT=${formData.get('inertial_port')} 
    }
    
    Rear_Bumper { 
    PORT=${formData.get('rear_bumper_port')} 
    } 
}

PRINTLOGO=${formData.has('print_logo')}
LOGTOFILE=${formData.has('log_to_file')}
MAXOPTIONSSIZE=${formData.get('max_options_size')}
POLLINGRATE=${formData.get('polling_rate')}
CTRLR1POLLINGRATE=${formData.get('ctrlr1_polling_rate')}
CONFIGTYPE=${formData.get('config_type')}
TEAMNUMBER=${formData.get('team_number')}
LOADINGGIFPATH=${formData.get('loading_gif_path')}
AUTOGIFPATH=${formData.get('auto_gif_path')}
DRIVEGIFPATH=${formData.get('drive_gif_path')}
CUSTOMMESSAGE=${formData.get('custom_message')}
DRIVEMODE=${formData.get('drive_mode')}
VERSION=${await getLatestRelease('RanchoDVT/Comp-V3')}`;

            (document.getElementById('copy-button')).style.display = 'inline-block';
        });
    }

    if (document.getElementById('copy-button')) {
        (document.getElementById('copy-button')).addEventListener('click', function () {
            if ((document.getElementById('config-output')).textContent) {
                navigator.clipboard.writeText((document.getElementById('config-output')).textContent)
                    .then(() => {
                        console.debug('Config copied to clipboard!');
                        const copyButton = document.getElementById('copy-button');
                        copyButton.innerHTML = 'Copied! ✅';
                        setTimeout(() => {
                            copyButton.innerHTML = 'Recopy';
                        }, 2000);
                    })
                    .catch(err => console.error('Error copying text:', err));
            }
        });
    }

    window.showPopup = async function (type) {
        let popupText, downloadLink;
        const downloadButton = document.getElementById('download-link');

        if (type === 'Dev') {
            popupText = 'Thank you for downloading Comp-V3 *dev*! This download link goes to the Github API. The download is the source code. <br><br>You will need my Custom SDK to use this. Check out my other download in the navbar.';
            downloadLink = 'https://github.com/RanchoDVT/Comp-V3/archive/refs/heads/dev.zip';
            document.getElementById('popup-title').innerText = `Download Comp-V3 ${type}`;
        } else if (type === 'Stable') {
            const latestTag = await getLatestRelease('RanchoDVT/Comp-V3');
            popupText = 'Thank you for downloading Comp-V3 stable! This download link goes to the Github API. The download is the source code. <br><br>You will need my Custom SDK to use this. Check out my other download in the navbar.';
            document.getElementById('popup-title').innerText = `Download Comp-V3 ${type} | ${latestTag}`;
            downloadLink = `https://github.com/RanchoDVT/Comp-V3/archive/refs/tags/${latestTag}.zip`;
            if (!latestTag || latestTag === 'Unknown') {
                disableDownloadButton(downloadButton);
                popupText = 'Release information not available. Please try again later.';
            }
        } else if (type === 'SDK') {
            const latestTag = await getLatestRelease('RanchoDVT/Vex-SDK');
            popupText = '*Vex Took this down! <br>You cannot download this for now!* <br><br>Thank you for downloading my custom SDK. <br><strong>This is unofficial and in no way affiliated with VEX Robotics. Please contact me for more info.</strong><br><a target="_blank" href="https://github.com/RanchoDVT/Vex-SDK/releases"><br>Source code</a><br><br><strong>The source code might not download correctly – you may have to use git clone.</strong><br>The download button is the simple powershell script.';
            downloadLink = `https://github.com/RanchoDVT/Vex-SDK/blob/dev/Vex-SDK.updater.ps1`;
            document.getElementById('popup-title').innerText = `Download Custom ${type} | ${latestTag}`;
            if (!latestTag || latestTag === 'Unknown') {
                disableDownloadButton(downloadButton);
                downloadLink = '#';
            }
        }

        // Set the popup text and download link
        document.getElementById('popup-text').innerHTML = popupText;
        downloadButton.href = downloadLink;

        // Finally, display the popup and overlay
        document.getElementById('popup').classList.add('active');
        document.getElementById('overlay').classList.add('active');
    };

    // Helper function to disable the download button
    function disableDownloadButton(button) {
        button.classList.add('disabled-download');
        button.removeAttribute('href');
        button.style.backgroundColor = 'gray';
        button.innerText = 'Download Unavailable';
    }
});

function hidePopup() {
    document.getElementById('popup').classList.remove('active');
    document.getElementById('overlay').classList.remove('active');
}
