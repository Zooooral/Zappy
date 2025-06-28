import {themes as prismThemes} from 'prism-react-renderer';
import type {Config} from '@docusaurus/types';
import type * as Preset from '@docusaurus/preset-classic';

const config: Config = {
  title: 'Zappy Documentation',
  tagline: 'Multi-language network game implementation',
  favicon: 'img/favicon.ico',

  url: 'https://your-organization.github.io',
  baseUrl: '/zappy/',

  organizationName: 'epitech',
  projectName: 'zappy',

  onBrokenLinks: 'throw',
  onBrokenMarkdownLinks: 'warn',

  i18n: {
    defaultLocale: 'en',
    locales: ['en'],
  },

  presets: [
    [
      'classic',
      {
        docs: {
          sidebarPath: './sidebars.ts',
          showLastUpdateAuthor: true,
          showLastUpdateTime: true,
        },
        blog: false,
        theme: {
          customCss: './src/css/custom.css',
        },
      } satisfies Preset.Options,
    ],
  ],

  themeConfig: {
    navbar: {
      title: 'Zappy',
      logo: {
        alt: 'Zappy Logo',
        src: 'img/logo.svg',
      },
      items: [
        {
          type: 'docSidebar',
          sidebarId: 'tutorialSidebar',
          position: 'left',
          label: 'Documentation',
        },
        {
          to: '/docs/user-guide/installation/system-requirements',
          label: 'User Guide',
          position: 'left',
        },
        {
          to: '/docs/developer-guide/overview',
          label: 'Developer Guide',
          position: 'left',
        },
        {
          href: 'https://github.com/your-org/zappy',
          label: 'GitHub',
          position: 'right',
        },
      ],
    },
    footer: {
      style: 'dark',
      links: [
        {
          title: 'User Documentation',
          items: [
            {
              label: 'Installation',
              to: '/docs/user-guide/installation/system-requirements',
            },
          ],
        },
        {
          title: 'Developer Documentation',
          items: [
            {
              label: 'Overview',
              to: '/docs/developer-guide/overview',
            },
            {
              label: 'AI Module',
              to: '/docs/developer-guide/ai/architecture',
            },
          ],
        },
      ],
      copyright: `Copyright © ${new Date().getFullYear()} Zappy Project Documentation.`,
    },
    prism: {
      theme: prismThemes.github,
      darkTheme: prismThemes.dracula,
      additionalLanguages: ['cpp', 'c', 'javascript', 'makefile', 'bash'],
    },
  } satisfies Preset.ThemeConfig,
};

export default config;
