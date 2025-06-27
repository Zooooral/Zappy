import type {SidebarsConfig} from '@docusaurus/plugin-content-docs';

const sidebars: SidebarsConfig = {
  tutorialSidebar: [
    'introduction',
    {
      type: 'category',
      label: 'User Guide',
      collapsed: false,
      items: [
        {
          type: 'category',
          label: 'Installation & Setup',
          items: [
            'user-guide/installation/system-requirements',
            'user-guide/installation/building-project',
          ],
        },
      ],
    },
    {
      type: 'category',
      label: 'Developer Guide',
      collapsed: false,
      items: [
        'developer-guide/overview',
        {
          type: 'category',
          label: 'AI Module (Node.js)',
          items: [
            'developer-guide/ai/architecture',
          ],
        },
      ],
    },
  ],
};

export default sidebars;
