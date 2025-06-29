import clsx from 'clsx';
import Link from '@docusaurus/Link';
import useDocusaurusContext from '@docusaurus/useDocusaurusContext';
import Layout from '@theme/Layout';
import Heading from '@theme/Heading';

import styles from './index.module.css';

function HomepageHeader() {
  const {siteConfig} = useDocusaurusContext();
  return (
    <header className={clsx('hero hero--primary', styles.heroBanner)}>
      <div className="container">
        <Heading as="h1" className="hero__title">
          {siteConfig.title}
        </Heading>
        <p className="hero__subtitle">{siteConfig.tagline}</p>
        <div className={styles.buttons}>
          <Link
            className="button button--secondary button--lg"
            to="/docs/introduction">
            Get Started
          </Link>
        </div>
      </div>
    </header>
  );
}

function ProjectInfo() {
  return (
    <section className={styles.projectInfo}>
      <div className="container">
        <div className="row">
          <div className="col col--8 col--offset-2">
            <div className="text--center margin-bottom--lg">
              <Heading as="h2">About Zappy</Heading>
              <p className="text--lg">
                A distributed network game implementation featuring three interconnected components:
                a high-performance C server, an interactive C++ GUI with 3D visualization, and intelligent Node.js AI clients.
                Teams compete on a tile-based world to gather resources and achieve maximum elevation.
              </p>
            </div>
            
            <div className="row margin-bottom--lg">
              <div className="col col--4">
                <div className="text--center">
                  <h4>🖥️ Server (C)</h4>
                  <p>High-performance game logic and network handling</p>
                </div>
              </div>
              <div className="col col--4">
                <div className="text--center">
                  <h4>🎮 GUI (C++)</h4>
                  <p>Real-time 3D visualization with Raylib</p>
                </div>
              </div>
              <div className="col col--4">
                <div className="text--center">
                  <h4>🤖 AI (Node.js)</h4>
                  <p>Autonomous strategy implementation</p>
                </div>
              </div>
            </div>

            <div className="text--center margin-bottom--lg">
              <p><strong>Epitech 2nd Year Project</strong></p>
              <p>📅 May 26 - June 29, 2025</p>
            </div>
          </div>
        </div>
      </div>
    </section>
  );
}

function Contributors() {
  const contributors = [
    'maxence.bunel@epitech.eu',
    'flavien.de-rigne@epitech.eu', 
    'victor.jurgens-mestre@epitech.eu',
    'yoan.gaudiero@epitech.eu',
    'aloys.mangin@epitech.eu',
    'pierre.lissope@gmail.com'
  ];

  return (
    <section className={styles.contributors}>
      <div className="container">
        <div className="row">
          <div className="col col--6 col--offset-3">
            <div className="text--center">
              <Heading as="h3">Contributors</Heading>
              <div className={styles.contributorsList}>
                {contributors.map((email, index) => (
                  <div key={index} className={styles.contributor}>
                    {email}
                  </div>
                ))}
              </div>
            </div>
          </div>
        </div>
      </div>
    </section>
  );
}

export default function Home(): JSX.Element {
  const {siteConfig} = useDocusaurusContext();
  return (
    <Layout
      title={`${siteConfig.title} - Multi-language Network Game`}
      description="Distributed network game with C server, C++ GUI, and Node.js AI clients">
      <HomepageHeader />
      <main>
        <ProjectInfo />
        
        <section className={styles.features}>
          <div className="container">
            <div className="row">
              <div className={clsx('col col--4')}>
                <div className="text--center padding-horiz--md">
                  <Heading as="h3">📚 User Guide</Heading>
                  <p>Installation instructions, system requirements, and game rules for end users</p>
                  <Link
                    className="button button--outline button--primary"
                    to="/docs/user-guide/installation/system-requirements">
                    View User Guide
                  </Link>
                </div>
              </div>
              <div className={clsx('col col--4')}>
                <div className="text--center padding-horiz--md">
                  <Heading as="h3">⚙️ Developer Guide</Heading>
                  <p>Comprehensive technical documentation, architecture overview, and development guidelines</p>
                  <Link
                    className="button button--outline button--primary"
                    to="/docs/developer-guide/overview">
                    View Developer Guide
                  </Link>
                </div>
              </div>
              <div className={clsx('col col--4')}>
                <div className="text--center padding-horiz--md">
                  <Heading as="h3">🔧 Quick Start</Heading>
                  <p>Build all components and start playing immediately with our quick setup guide</p>
                  <Link
                    className="button button--outline button--primary"
                    to="/docs/user-guide/installation/building-project">
                    Quick Start
                  </Link>
                </div>
              </div>
            </div>
            
            <div className="row margin-top--lg">
              <div className={clsx('col col--6')}>
                <div className="text--center padding-horiz--md">
                  <Heading as="h4">🚀 Architecture</Heading>
                  <p>Learn about the distributed system design and component interactions</p>
                  <Link
                    className="button button--secondary button--sm"
                    to="/docs/introduction">
                    View Architecture
                  </Link>
                </div>
              </div>
              <div className={clsx('col col--6')}>
                <div className="text--center padding-horiz--md">
                  <Heading as="h4">🎯 Game Rules</Heading>
                  <p>Understand the game mechanics, objectives, and winning conditions</p>
                  <Link
                    className="button button--secondary button--sm"
                    to="/docs/user-guide/gameplay/rules">
                    View Rules
                  </Link>
                </div>
              </div>
            </div>
          </div>
        </section>

        <Contributors />
      </main>
    </Layout>
  );
}