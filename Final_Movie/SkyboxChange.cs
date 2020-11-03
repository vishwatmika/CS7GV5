using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SkyboxChange : MonoBehaviour
{
    public Material sky1;
    public Material sky2;
    public ParticleSystem particles;

    // Start is called before the first frame update
    void Start()
    {
        RenderSettings.skybox = sky1;
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.K))
            RenderSettings.skybox = sky2;
        if (Input.GetKeyDown(KeyCode.L))
            RenderSettings.skybox = sky1;
        if (Input.GetKeyDown(KeyCode.P))
            particles.Play();
    }
}
