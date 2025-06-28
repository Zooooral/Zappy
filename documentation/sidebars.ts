import type {SidebarsConfig} from '@docusaurus/plugin-content-docs';

const sidebars: SidebarsConfig = {
  tutorialSidebar: [
    'introduction',
    {
      type: 'category',
      label: 'User Guide',
      items: [
        {
          type: 'category',
          label: 'Installation',
          items: [
            'user-guide/installation/system-requirements',
            'user-guide/installation/building-project',
            'user-guide/installation/quick-start',
            'user-guide/installation/advanced-setup',
          ],
        },
        {
          type: 'category',
          label: 'Gameplay',
          items: [
            'user-guide/gameplay/rules',
          ],
        },
      ],
    },
    {
      type: 'category',
      label: 'Developer Guide',
      items: [
        'developer-guide/overview',
        {
          type: 'category',
          label: 'AI Module',
          items: [
            'developer-guide/ai/architecture',
            'developer-guide/ai/communication-protocol',
          ],
        },
        {
          type: 'category',
          label: 'GUI Module',
          items: [
            'developer-guide/gui/architecture',
            'developer-guide/gui/network-communication',
            'developer-guide/gui/file-structure',
          ],
        },
        {
          type: 'category',
          label: 'Server Module',
          items: [
            'developer-guide/server/architecture',
            'developer-guide/server/protocol',
          ],
        },
      ],
    },
    {
      type: 'category',
      label: 'API Reference',
      items: [
        'api-reference/server-api',
        'api-reference/gui-api',
      ],
    },
  ],
};

export default sidebars;
