using System.Collections;
using UnityEngine;
using UnityEngine.InputSystem;

public class plainSpawn : MonoBehaviour
{

    public float spawnPointY = 4;

    public float tiltAngle = 30f;       // угол наклона вокруг локальной оси Z (в градусах)
    public float tiltDuration = 0.8f;   // время поворота в секунды
    public float tiltHoldTime = 2f;     // как долго держать наклон (чтобы кубы съехали)


    public GameObject spawnedCapsule;
    public GameObject spawnedSphere;

    private Renderer _planeRenderer;
    private bool _isTilting = false;

    void Start()
    {
        _planeRenderer = GetComponent<Renderer>();
    }
    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.Q))
        {
            Debug.Log("qqq");
            var spawnPos = GetSpawnPoint();
            Instantiate(spawnedCapsule, spawnPos, Quaternion.identity);
        }
        if (Input.GetKeyDown(KeyCode.Space))
        {
            var spawnPos = GetSpawnPoint();
            Instantiate(spawnedSphere, spawnPos, Quaternion.identity);
        }
        if (Input.GetKeyDown(KeyCode.W) && !_isTilting)
        {
            StartCoroutine(TiltPlane(tiltAngle, tiltDuration, tiltHoldTime));
        }
    }

    private Vector3 GetSpawnPoint()
    {

        Bounds b = _planeRenderer.bounds; 
        float minX = b.min.x;
        float maxX = b.max.x;
        float minZ = b.min.z;
        float maxZ = b.max.z;

        float x = Random.Range(minX, maxX);
        float z = Random.Range(minZ, maxZ);
        float y = b.max.y + spawnPointY;

        return new Vector3(x, y, z);
    }
    private IEnumerator TiltPlane(float angle, float duration, float holdTime)
    {
        Debug.Log("ww");
        _isTilting = true;

        Quaternion startRot = transform.rotation;
        Quaternion targetRot = startRot * Quaternion.Euler(0f, 0f, angle);

        float t = 0f;
        while (t < duration)
        {
            t += Time.deltaTime;
            transform.rotation = Quaternion.Slerp(startRot, targetRot, t / duration);
            yield return null;
        }
        transform.rotation = targetRot;

        yield return new WaitForSeconds(holdTime);

        t = 0f;
        while (t < duration)
        {
            t += Time.deltaTime;
            transform.rotation = Quaternion.Slerp(targetRot, startRot, t / duration);
            yield return null;
        }
        transform.rotation = startRot;

        _isTilting = false;
    }
}
